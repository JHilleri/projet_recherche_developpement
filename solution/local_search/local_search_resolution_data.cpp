#include "local_search_resolution_data.h"
#include "branch_and_bound.h"

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
			return first_depature_date + ((batch.get_jobs().size() + static_cast<size_t>(machine_count) + 1) * longest_task_duration * 3) + 99999;
		}
	}

	local_search_resolution_data::local_search_resolution_data(solution & solution_to_work, index batch_to_solve, time departure_window_begining, std::vector<time> const & earliest_production_start) :
		earliest_production_start(earliest_production_start),
		result(solution_to_work),
		batch(result.get_instance()->get_batchs()[batch_to_solve]),
		departure_window_begining(departure_window_begining),
		batch_to_solve(batch_to_solve),
		machine_count(solution_to_work.get_instance()->get_machine_count())
	{
		departure_window_ending = get_last_possible_departure(batch, departure_window_begining, machine_count);
		const auto & jobs = solution_to_work.get_instance()->get_batchs()[batch_to_solve].get_jobs();
		std::vector<int> jobs_order(jobs.size());

		std::transform(jobs.begin(), jobs.end(), jobs_order.begin(), [](job const & job_to_read) {return job_to_read.get_index(); });
		delivery_cost_per_departure = Branch_and_bound{ *result.get_instance() }.generate_fct_with_branch_and_bound(jobs_order, departure_window_begining, departure_window_ending);
	}

}