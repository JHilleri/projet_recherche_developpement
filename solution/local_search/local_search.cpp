#pragma once
#include "local_search.h"
#include "linear_function.h"
#include "branch_and_bound.h"
#include <algorithm>
#include <numeric>
#include <functional>
#include "batch_solution.h"
#include "common.h"

namespace solver
{

	solution local_search::operator()(const_instance_ptr instance_to_solve)
	{
		solution result(instance_to_solve);

		index const index_last_machine = instance_to_solve->get_machine_count() - 1;
		index const batch_count = instance_to_solve->get_batchs().size();

		result.set_planned_batchs(std::vector<planned_batch>(batch_count));

		std::vector<time> production_beginings(instance_to_solve->get_machine_count(), 0);
		time departure_window = 0;
		for (index index_batch = 0; index_batch <batch_count; ++index_batch)
		{
			solve_batch(instance_to_solve, index_batch, result, departure_window, production_beginings);

			planned_job const & last_planned_job = result.get_planned_batchs()[index_batch].get_jobs().back();
			departure_window = last_planned_job.get_production_date_on_machine(index_last_machine) + last_planned_job.get_job()->get_duration_on_machine(index_last_machine);
			for (index index_machine = 0; index_machine < instance_to_solve->get_machine_count(); ++index_machine)
			{
				production_beginings[index_machine] = last_planned_job.get_production_date_on_machine(index_machine) + last_planned_job.get_job()->get_duration_on_machine(index_machine);
			}
		}

		cost delivery_cost = std::accumulate(result.get_planned_batchs().begin(), result.get_planned_batchs().end(), 0, planned_batch_delivery_cost_accumulator);
		cost inventory_cost = std::accumulate(result.get_planned_batchs().begin(), result.get_planned_batchs().end(), 0, planned_batch_inventory_cost_accumulator);

		result.set_delivery_cost(delivery_cost);
		result.set_score(delivery_cost + inventory_cost);

		return result;
	}

	void local_search::solve_batch(const_instance_ptr instance_to_solve, index batch_index, solution & output, time earlyer_possible_departure, std::vector<time> const & earliest_production_start)
	{
		auto instance_data = std::make_shared<local_search_resolution_data>(output, batch_index, earlyer_possible_departure, earliest_production_start);
		std::chrono::milliseconds minimum_batch_resolution_duration = minimum_duration / instance_to_solve->get_batchs().size();

		std::chrono::time_point batch_resulution_begining = std::chrono::system_clock::now();

		batch_solution best_found_solution( instance_data, earliest_production_start);
		best_found_solution.evaluate_score();
		do
		{
			batch_solution current_solution(best_found_solution);
			current_solution.shuffle();
			current_solution.evaluate_score();
			bool has_changed;
			do
			{
				has_changed = false;
				std::vector<batch_solution> neighbourhood = m_neighbourhood_provider(current_solution);
				decltype(neighbourhood)::iterator best_neighbor = m_neighbourhoud_explorator(current_solution, neighbourhood);
				if (best_neighbor != neighbourhood.end())
				{
					std::swap(*best_neighbor, current_solution);
					has_changed = true;
				}
			} while (has_changed);
			if (best_found_solution.get_score() > current_solution.get_score())
			{
				best_found_solution = current_solution;

			}
		} while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()  - batch_resulution_begining) < minimum_batch_resolution_duration);
		

		index job_count = instance_to_solve->get_batchs()[batch_index].get_jobs().size();
		std::vector<planned_job> planned_jobs(job_count);

		for (index index_job = 0; index_job < best_found_solution.get_jobs().size(); ++index_job)
		{
			auto job_ptr = instance_to_solve->get_job_by_index(best_found_solution.get_jobs()[index_job]->get_index());

			std::vector<time> prodution_dates(instance_to_solve->get_machine_count());
			std::transform(
				best_found_solution.get_task_begin().begin(),
				best_found_solution.get_task_begin().end(),
				prodution_dates.begin(), 
				[index_job](std::vector<time> const & machine_tasks_begining_dates) -> time
				{
					return machine_tasks_begining_dates[index_job]; 
				}
			);

			planned_jobs[index_job] = planned_job(job_ptr, prodution_dates);
		}
		

		planned_batch batch_result;
		batch_result.set_jobs(planned_jobs);
		batch_result.set_delivery_cost(best_found_solution.get_delivery_cost());
		output.get_planned_batchs()[batch_index] = batch_result;
	}
}
