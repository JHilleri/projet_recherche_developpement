#include "solution_writer.h"
#include "util.hpp"

namespace solver
{
	namespace
	{
		void write_job(std::ostream & output, planned_job const & job_to_write, index machine_count)
		{
			output << "{\"index\":" << job_to_write.get_job()->get_index();
			output << ",\"tasks\": [";
			if (machine_count > 0)
			{
				output << "{\"begining\": " << job_to_write.get_production_date_on_machine(0) << ", \"end\": " << job_to_write.get_production_date_on_machine(0) + job_to_write.get_job()->get_duration_on_machine(0) << "}";
			}
			for (index index_machine = 1; index_machine < machine_count; ++index_machine)
			{
				output << ",{\"begining\": " << job_to_write.get_production_date_on_machine(index_machine) << ", \"end\": " << job_to_write.get_production_date_on_machine(index_machine) + job_to_write.get_job()->get_duration_on_machine(index_machine) << "}";
			}
			output << "]}";
		}
		void write_batch(std::ostream & output, planned_batch const & batch_to_write, index machine_count)
		{
			output << "{\"jobs\": [";
			for_each_with_separator(
				batch_to_write.get_jobs().begin(),
				batch_to_write.get_jobs().end(), 
				[&](planned_job const & job_to_write) { write_job(output, job_to_write, machine_count); },
				[&](planned_job const & job_to_write) { output << ","; }
			);
			output << "]}";
		}
	}

	void solver::solution_writer::write(std::ostream & output, solution & solution_to_write)
	{
		output << "{\"machine_count\": " << solution_to_write.get_instance()->get_machine_count() << ",\"score\": " << solution_to_write.get_score() << ", \"job_count\": " << solution_to_write.get_instance()->get_job_count() << ", \"batchs\": [";
		for_each_with_separator(solution_to_write.get_planned_batchs().begin(), solution_to_write.get_planned_batchs().end(), [&](auto const & batch)
		{
			write_batch(output, batch, solution_to_write.get_instance()->get_machine_count());
		}, [&](auto const & batch)
		{
			output << ",";
		});
		output << "]}" << std::endl;
	}
	void solution_writer::write(std::ostream & output, solution_with_statistics & solution_to_write)
	{
		output << "{\"duration\":" << solution_to_write.get_process_duration().count() << ",\"solution\":";
		write(output, static_cast<solution&>(solution_to_write));
		output << "}" << std::endl;
	}
}
