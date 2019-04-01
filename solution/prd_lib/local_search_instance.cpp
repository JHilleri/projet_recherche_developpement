#include "local_search_instance.h"

namespace solver
{
	namespace
	{

		time get_last_possible_departure(batch const & batch, time first_depature_date, index machine_count)
		{
			if (batch.get_jobs().empty()) return first_depature_date;

			time longest_task_duration = *std::max(batch.get_jobs()[0]->get_duration_per_machine().begin(), batch.get_jobs()[0]->get_duration_per_machine().begin());
			for (auto const & job : batch.get_jobs())
			{
				for (index index_machine = 0; index_machine < machine_count; ++index_machine)
				{
					auto job_longest_task_duration = *std::max(job->get_duration_per_machine().begin(), job->get_duration_per_machine().begin());
					if (job_longest_task_duration > longest_task_duration)
						longest_task_duration = job_longest_task_duration;
				}
			}
			return first_depature_date + (std::max(batch.get_jobs().size(), static_cast<size_t>(machine_count)) + 1) * longest_task_duration * 2 + 1000;
		}
	}

	local_search_instance::local_search_instance(solution & solution_to_work, index batch_to_solve, time departure_window_begining) : 
		result( solution_to_work ), 
		batch( result.get_instance()->get_batchs()[batch_to_solve] ), 
		departure_window_begining( departure_window_begining ),
		batch_to_solve( batch_to_solve ),
		machine_count( solution_to_work.get_instance()->get_machine_count() ),
		current_solution (
			{ batch.get_jobs().begin(), batch.get_jobs().end() }, 
			{ machine_count, std::vector<time>(batch.get_jobs().size()) }, 
			*this )
	{
		departure_window_ending = get_last_possible_departure(batch, departure_window_begining, machine_count);
		std::vector<int> jobs_order(current_solution.get_jobs().size());
		std::transform(current_solution.get_jobs().begin(), current_solution.get_jobs().end(), jobs_order.begin(), [](job const & job_to_read) {return job_to_read.get_index(); });
		delivery_cost_per_departure = Branch_and_bound{ *result.get_instance() }.generate_fct_with_branch_and_bound(jobs_order, departure_window_begining, departure_window_ending);
	}

	cost local_search_instance::get_current_score() const
	{
		return current_solution.get_score();
	}

	std::vector<permutation> local_search_instance::get_current_permutations() const
	{
		std::vector<permutation> permutations;
		permutations.reserve(current_solution.get_jobs().size()*2-1);
		for (index index_job_1 = 0, job_count = current_solution.get_jobs().size(); index_job_1 < job_count; ++index_job_1)
		{
			for (index index_job_2 = index_job_1 + 1; index_job_2 < job_count; ++index_job_2)
			{
				permutations.emplace_back(index_job_1, index_job_2);
			}
		}
		return permutations;
	}

	void local_search_instance::permutate(permutation permutation_to_perform) {
		current_solution = batch_solution(current_solution, permutation_to_perform);
	}

	cost local_search_instance::evaluate_permutation(permutation permutation_to_evaluate)
	{
		return batch_solution(current_solution, permutation_to_evaluate).get_score();
	}

	batch_solution const & local_search_instance::get_current_batch_solution()
	{
		return current_solution;
	}

	cost batch_solution::get_score() const
	{
		cost delivery_cost = get_delivery_cost();
		cost total_in_progress_inventory_cost = get_total_in_progress_inventory_cost();
		cost total_completed_inventory_cost = get_total_ended_inventory_cost();
		return delivery_cost + total_in_progress_inventory_cost + total_completed_inventory_cost;
	}

	cost batch_solution::get_delivery_cost() const
	{
		time end = tasks_end[current_local_search_instance.get().machine_count - 1][jobs.size() - 1] + current_local_search_instance.get().departure_window_begining;
		return current_local_search_instance.get().delivery_cost_per_departure.eval_fct_lin(end);
	}

	cost batch_solution::get_inventory_cost() const
	{
		return get_total_in_progress_inventory_cost() + get_total_ended_inventory_cost();
	}

	batch_solution::batch_solution(std::vector<job> const & jobs, std::vector<std::vector<time>> const & delays, local_search_instance & local_search_instance) :
		jobs(jobs),
		delays(delays),
		current_local_search_instance(local_search_instance)
	{
		update_tasks_dates();
	}

	batch_solution::batch_solution(batch_solution const & base, permutation permutation_to_perform) :
		jobs(base.jobs),
		delays(base.delays),
		current_local_search_instance(base.current_local_search_instance)
	{
		//std::swap(jobs[permutation_to_perform.first], jobs[permutation_to_perform.second]);
		job tmp(std::move(jobs[permutation_to_perform.first]));
		jobs[permutation_to_perform.first] = std::move(jobs[permutation_to_perform.second]);
		jobs[permutation_to_perform.second] = std::move(tmp);
		update_tasks_dates();
	}

	std::vector<job> const & batch_solution::get_jobs() const
	{
		return jobs;
	}

	std::vector<std::vector<time>> const & batch_solution::get_delays() const
	{
		return delays;
	}

	std::vector<std::vector<time>> const & batch_solution::get_task_end() const
	{
		return tasks_end;
	}

	std::vector<std::vector<time>> const & batch_solution::get_task_begin() const
	{
		return tasks_begining;
	}

	cost batch_solution::get_total_in_progress_inventory_cost() const
	{
		cost total_in_progress_inventory_cost = 0;
		for (index index_machine = 0; index_machine < current_local_search_instance.get().machine_count - 1; ++index_machine)
		{
			for (index index_job = 0; index_job <jobs.size(); ++index_job)
			{
				time task_in_progress_inventory_time = tasks_begining[index_machine + 1][index_job] - tasks_end[index_machine][index_job];
				cost task_in_progress_inventory_cost = task_in_progress_inventory_time * jobs[index_job].get_in_progress_inventory_cost()[index_machine];
				total_in_progress_inventory_cost += task_in_progress_inventory_cost;
			}
		}
		return total_in_progress_inventory_cost;
	}

	cost batch_solution::get_total_ended_inventory_cost() const
	{
		cost total_ended_inventory_cost = 0;
		for (index index_job = 0; index_job <jobs.size(); ++index_job)
		{
			time inventory_time = tasks_end[current_local_search_instance.get().machine_count - 1][jobs.size() - 1] - tasks_end[current_local_search_instance.get().machine_count - 1][index_job];
			total_ended_inventory_cost += inventory_time * jobs[index_job].get_ended_inventory_cost();
		}
		return total_ended_inventory_cost;
	}

	void batch_solution::update_tasks_dates()
	{
		decltype(tasks_begining) task_begining(current_local_search_instance.get().machine_count, std::vector<time>(jobs.size(), 0));
		decltype(tasks_end) task_ending(current_local_search_instance.get().machine_count, std::vector<time>(jobs.size(), 0));

		time begining = current_local_search_instance.get().departure_window_begining;

		// set begining and end for all task on the first machine
		for (index index_job = 0, next_start = begining; index_job < jobs.size(); ++index_job)
		{
			task_begining[0][index_job] = next_start + delays[0][index_job];
			task_ending[0][index_job] = task_begining[0][index_job] + jobs[index_job].get_duration_on_machine(0);
			next_start = task_ending[0][index_job];
		}

		// set begining and end of all task of the first job;
		for (index index_machine = 0, next_start = begining; index_machine < current_local_search_instance.get().machine_count; ++index_machine)
		{
			task_begining[index_machine][0] = next_start + delays[index_machine][0];
			task_ending[index_machine][0] = task_begining[index_machine][0] + jobs[0].get_duration_on_machine(index_machine);
			next_start = task_ending[index_machine][0];
		}

		for (index index_job = 1; index_job < jobs.size(); ++index_job)
		{
			for (index index_machine = 1; index_machine < current_local_search_instance.get().machine_count; ++index_machine)
			{
				time end_of_job_previous_task = task_ending[index_machine-1][index_job];
				time end_of_machine_previous_task = task_ending[index_machine][index_job - 1];

				task_begining[index_machine][index_job] = std::max(end_of_job_previous_task, end_of_machine_previous_task) + delays[index_machine][index_job];
				task_ending[index_machine][index_job] = task_begining[index_machine][index_job] + jobs[index_job].get_duration_on_machine(index_machine);
			}
		}

		std::swap(tasks_begining, task_begining);
		std::swap(tasks_end, task_ending);
	}

}