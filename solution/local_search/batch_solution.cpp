#include "batch_solution.h"
#include <random>

namespace solver
{
	cost batch_solution::get_score() const
	{
		cost delivery_cost = get_delivery_cost();
		cost total_in_progress_inventory_cost = get_total_in_progress_inventory_cost();
		cost total_completed_inventory_cost = get_total_ended_inventory_cost();
		return delivery_cost + total_in_progress_inventory_cost + total_completed_inventory_cost;
	}

	cost batch_solution::get_delivery_cost() const
	{
		time end = tasks_end[resolution_data->machine_count - 1][jobs.size() - 1] + resolution_data->departure_window_begining;
		return resolution_data->delivery_cost_per_departure.eval_fct_lin(end);
	}

	cost batch_solution::get_inventory_cost() const
	{
		return get_total_in_progress_inventory_cost() + get_total_ended_inventory_cost();
	}

	batch_solution::batch_solution(std::shared_ptr<const local_search_resolution_data> local_search_instance, std::vector<time> const & earliest_production_start) :
		jobs(local_search_instance->batch.get_jobs()),
		delays(local_search_instance->machine_count, std::vector<time>(local_search_instance->batch.get_jobs().size())),
		resolution_data(local_search_instance),
		earliest_production_start(earliest_production_start)
	{
	}

	std::vector<std::vector<time>> const & batch_solution::get_task_end() const
	{
		return tasks_end;
	}

	std::vector<std::vector<time>> const & batch_solution::get_task_begin() const
	{
		return tasks_begining;
	}

	void batch_solution::shuffle()
	{
		static auto random = std::default_random_engine();
		std::shuffle(jobs.begin(), jobs.end(), random);
	}

	cost batch_solution::evaluate_score()
	{
		update_tasks_dates();
		score = get_inventory_cost() + get_delivery_cost();
		return score;
	}

	cost batch_solution::get_total_in_progress_inventory_cost() const
	{
		cost total_in_progress_inventory_cost = 0;
		for (index index_machine = 1; index_machine < resolution_data->machine_count - 1; ++index_machine)
		{
			for (index index_job = 0; index_job < jobs.size(); ++index_job)
			{
				time task_in_progress_inventory_time = tasks_begining[index_machine][index_job] - tasks_end[index_machine - 1][index_job];
				cost task_in_progress_inventory_cost = task_in_progress_inventory_time * jobs[index_job]->get_in_progress_inventory_cost()[index_machine - 1];
				total_in_progress_inventory_cost += task_in_progress_inventory_cost;
			}
		}
		return total_in_progress_inventory_cost;
	}

	cost batch_solution::get_total_ended_inventory_cost() const
	{
		cost total_ended_inventory_cost = 0;
		for (index index_job = 0; index_job < jobs.size(); ++index_job)
		{
			time inventory_time = tasks_end[resolution_data->machine_count - 1][jobs.size() - 1] - tasks_end[resolution_data->machine_count - 1][index_job];
			total_ended_inventory_cost += inventory_time * jobs[index_job]->get_ended_inventory_cost();
		}
		return total_ended_inventory_cost;
	}

	void batch_solution::update_tasks_dates()
	{
		decltype(tasks_begining) task_begining(resolution_data->machine_count, std::vector<time>(jobs.size(), 0));
		decltype(tasks_end) task_ending(resolution_data->machine_count, std::vector<time>(jobs.size(), 0));

		time begining = resolution_data->departure_window_begining;

		// set begining and end for all task on the first machine
		{
			time end_of_previous_task = earliest_production_start.at(0);
			for (index index_job = 0; index_job < jobs.size(); ++index_job)
			{
				task_begining[0][index_job] = end_of_previous_task + delays[0][index_job];
				task_ending[0][index_job] = task_begining[0][index_job] + jobs[index_job]->get_duration_on_machine(0);
				end_of_previous_task = task_ending[0][index_job];
			}
		}

		// set begining and end of all task of the first job;
		{
			time end_of_previous_task = earliest_production_start.at(0);
			for (index index_machine = 0; index_machine < resolution_data->machine_count; ++index_machine)
			{
				task_begining[index_machine][0] = std::max(earliest_production_start.at(index_machine), end_of_previous_task) + delays[index_machine][0];
				task_ending[index_machine][0] = task_begining[index_machine][0] + jobs[0]->get_duration_on_machine(index_machine);
				end_of_previous_task = task_ending[index_machine][0];
			}
		}

		for (index index_job = 1; index_job < jobs.size(); ++index_job)
		{
			for (index index_machine = 1; index_machine < resolution_data->machine_count; ++index_machine)
			{
				time end_of_job_previous_task = task_ending[index_machine - 1][index_job];
				time end_of_machine_previous_task = task_ending[index_machine][index_job - 1];

				task_begining[index_machine][index_job] = std::max(end_of_job_previous_task, end_of_machine_previous_task) + delays[index_machine][index_job];
				task_ending[index_machine][index_job] = task_begining[index_machine][index_job] + jobs[index_job]->get_duration_on_machine(index_machine);
			}
		}

		std::swap(tasks_begining, task_begining);
		std::swap(tasks_end, task_ending);
	}
}