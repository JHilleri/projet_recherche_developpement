#pragma once
#include "local_search.h"
#include "linear_function.h"
#include "branch_and_bound.h"
#include <algorithm>
#include "local_search_instance.h"

namespace solver
{
	namespace
	{
		index const job_to_index(const_job_ptr & job)
		{
			return job->get_index();
		}

		inline std::pair<time, time> get_departure_window(const_instance_ptr instance_to_solve, index batch_to_solve_index, solution const & result)
		{
			time begining_of_batch_production = 0;
			if (batch_to_solve_index > 0)
			{
				auto const & previous_batch_last_job = result.get_planned_batchs()[batch_to_solve_index - 1].get_jobs().back();
				begining_of_batch_production = previous_batch_last_job.get_production_date_on_machine(instance_to_solve->get_machine_count());
			}
			batch const & batch_to_solve = instance_to_solve->get_batchs()[batch_to_solve_index];

			time batch_longest_task_duration;
			for (auto const & job : batch_to_solve.get_jobs())
			{
				auto const & durations = job->get_duration_per_machine();
				auto job_longest_task_duration = *std::max_element(durations.begin(), durations.end());
				batch_longest_task_duration = std::max(batch_longest_task_duration, job_longest_task_duration);
			}

			time departure_window_end = begining_of_batch_production + batch_longest_task_duration * (instance_to_solve->get_job_count() + instance_to_solve->get_machine_count());
			return std::make_pair(begining_of_batch_production, departure_window_end);
		}
		time get_last_possible_departure(batch const & batch, time first_depature_date, index machine_count)
		{
			if (batch.get_jobs().empty()) return first_depature_date;

			time longest_task_duration = 0;
			for (auto const & job : batch.get_jobs())
			{
				for (index index_machine = 0; index_machine < machine_count; ++index_machine)
				{
					auto job_longest_task_duration = *std::max(job->get_duration_per_machine().begin(), job->get_duration_per_machine().begin());
					if (job_longest_task_duration > longest_task_duration)
						longest_task_duration = job_longest_task_duration;
				}
			}
			return first_depature_date + (std::max(batch.get_jobs().size(), static_cast<size_t>(machine_count)) + 1) * longest_task_duration * 2;
		}
	}



	solution local_search::operator()(const_instance_ptr instance_to_solve)
	{
		solution result(instance_to_solve);

		std::vector<planned_batch> batchs(instance_to_solve->get_batchs().size());

		time departure_window = 0;
		for (index index_batch = 0; index_batch < batchs.size(); ++index_batch)
		{


		}

		return result;
	}


	void local_search::solve_batch(const_instance_ptr instance_to_solve, index batch_index, solution & output, time earlyer_possible_departure)
	{
		local_search_instance instance(output, batch_index, earlyer_possible_departure);
		index job_count = instance_to_solve->get_batchs()[batch_index].get_jobs().size();

		using scored_permutation = std::pair<std::pair<index, index>, cost>;
		auto const scored_permutation_comparator = [](auto const & permutation_1, auto const & permutation_2) -> bool{ return std::get<1>(permutation_1) < std::get<1>(permutation_2); };
		auto const scored_permutation_generator = [&instance](auto const & permutation_proposal) -> scored_permutation {return { permutation_proposal, instance.evaluate_permutation(permutation_proposal) }; };

		bool has_changed;
		do
		{
			has_changed = false;
			cost current_score = instance.get_current_score();
			auto permutations = instance.get_current_permutations();
			std::vector<scored_permutation> permutations_scores(job_count);
			std::transform(permutations.begin(), permutations.end(), permutations_scores.begin(), scored_permutation_generator);
			auto best_permutation = *std::min_element(permutations_scores.begin(), permutations_scores.end(), scored_permutation_comparator);
			if (best_permutation.second < current_score)
			{
				instance.permutate(best_permutation.first);
				has_changed = true;
			}

		} while (has_changed);

		auto instance_jobs = instance.get_current_batch_solution().get_jobs();
		std::vector<planned_job> jobs(job_count);
		std::transform(instance_jobs.begin(), instance_jobs.end(), jobs.begin(), [&instance_to_solve](job const &job_to_plan)->planned_job {
			auto job_ptr = *instance_to_solve->get_job_by_index(job_to_plan.get_index());
			return planned_job(job_ptr, )
		});
		planned_batch batch_result;
		batch_result.set_jobs();
		output.set_planned_batchs();
	}
}
