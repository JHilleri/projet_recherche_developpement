#include <string>
#include <algorithm>
#include <numeric>

#include "linear_function.h"
#include "branch_and_bound.h"
#include "instance.h"
#include "solution.h"
#include "solver_types.h"
#include "common.h"

namespace solver
{
	std::pair<time, time> get_departure_window(const_instance_ptr instance_to_solve, index batch_to_solve_index, solution const & result)
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
	index const job_ptr_to_index(const_job_ptr const & job)
	{
		return job->get_index();
	}
	index const job_to_index(job const & job)
	{
		return job.get_index();
	}
}