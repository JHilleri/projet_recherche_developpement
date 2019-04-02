#pragma once
#include "local_search.h"
#include "linear_function.h"
#include "branch_and_bound.h"
#include <algorithm>
#include <numeric>
#include "local_search_instance.h"
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
		result.set_inventory_cost(inventory_cost);
		result.set_score(delivery_cost + inventory_cost);

		return result;
	}


	namespace
	{
		std::vector<batch_solution> get_neighbourhood(batch_solution const & current_solution)
		{
			auto jobs_count = current_solution.get_jobs().size();
			std::vector<batch_solution> neighbourhood(jobs_count * jobs_count - 1, current_solution);
			for (index index_1 = 0; index_1 < jobs_count; ++index_1)
			{
				for (index index_2 = 0; index_2 < jobs_count; ++index_2)
				{
					if (index_1 != index_2)
					{
						auto & neighbor = neighbourhood[index_1 * jobs_count + index_2];
						std::swap(neighbor.get_jobs()[index_1], neighbor.get_jobs()[index_2]);
					}
				}
			}

			return neighbourhood;
		}

		auto get_best_neighbor(batch_solution const & current_solution, std::vector<batch_solution> & neighbourhood)
		{
			return std::find_if(neighbourhood.begin(), neighbourhood.end(), [&](batch_solution & neighbor) {
				cost score = neighbor.evaluate_score();
				return (score < current_solution.get_score());
			});
		}
	}


	void local_search::solve_batch(const_instance_ptr instance_to_solve, index batch_index, solution & output, time earlyer_possible_departure, std::vector<time> const & earliest_production_start)
	{

		local_search_instance instance(output, batch_index, earlyer_possible_departure, earliest_production_start);
		index job_count = instance_to_solve->get_batchs()[batch_index].get_jobs().size();
		std::chrono::milliseconds minimum_batch_resolution_duration = minimum_duration / instance_to_solve->get_batchs().size();

		using scored_permutation = std::pair<std::pair<index, index>, cost>;
		auto const scored_permutation_comparator = [](auto const & permutation_1, auto const & permutation_2) -> bool{ return std::get<1>(permutation_1) < std::get<1>(permutation_2); };
		auto const scored_permutation_generator = [&instance](auto const & permutation_proposal) -> scored_permutation {return { permutation_proposal, instance.evaluate_permutation(permutation_proposal) }; };

		std::chrono::time_point batch_resulution_begining = std::chrono::system_clock::now();
		batch_solution best_found_solution = instance.get_current_batch_solution();
		do
		{
			instance.shuffle();
			bool has_changed;
			do
			{
				has_changed = false;
				cost current_score = instance.get_current_score();
				//std::cerr << "lot " << batch_index << ", score : " << current_score << std::endl;
				std::vector<batch_solution> neighbourhood;
				auto permutations = instance.get_current_permutations();
				auto best_permutation = std::find_if(permutations.begin(), permutations.end(), [&instance, current_score](auto const & permutation) -> bool {
					auto score = instance.evaluate_permutation(permutation);
					return score < current_score;
				});
				if (best_permutation != permutations.end())
				{ 
					instance.permutate(*best_permutation); 
					has_changed = true;
				}
			} while (has_changed);
			//std::cerr << "minimum local, lot " << batch_index << ", score : "  << instance.get_current_score() << std::endl;
			if (best_found_solution.get_score() > instance.get_current_score())
			{
				best_found_solution = instance.get_current_batch_solution();
			}
		} while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()  - batch_resulution_begining) < minimum_batch_resolution_duration);
		

		std::vector<planned_job> planned_jobs(job_count);

		//auto proposed_batch = instance.get_current_batch_solution();
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
		// batch_result.set_inventory_cost(best_found_solution.get_inventory_cost());
		output.get_planned_batchs()[batch_index] = batch_result;
	}
}
