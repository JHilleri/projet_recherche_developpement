#include "solution_validator.h"
#include "solver_types.h"

#include <algorithm>
#include <numeric>
#include <iostream>

namespace solver
{
	namespace {
		inline time get_end_of_task(const planned_job &job, index machine_index)
		{
			return job.get_production_date_on_machine(machine_index) + job.get_job()->get_duration_on_machine(machine_index);
		}

		inline bool is_job_waiting_end_of_machine_previous_task(planned_job const & job_1, planned_job const & job_2, index machine_index)
		{
			return (get_end_of_task(job_1, machine_index) <= job_2.get_production_date_on_machine(machine_index));
		}

		inline bool is_machine_waiting_end_of_job_previous_task(planned_job const & job, index previous_machine_index, index machine_index)
		{
			return (get_end_of_task(job, previous_machine_index) <= job.get_production_date_on_machine(machine_index));
		}

		inline bool check_no_empty_batch(solution const & solution_to_check)
		{
			return(std::none_of(solution_to_check.get_planned_batchs().begin(), solution_to_check.get_planned_batchs().end(), [](planned_batch const & batch) {return batch.get_jobs().empty(); }));
		}

		inline bool check_no_batchs_production_colision(solution const & solution_to_check)
		{
			if (solution_to_check.get_planned_batchs().size() <= 1) return true;
			index const machine_count{ solution_to_check.get_instance()->get_machine_count() };
			std::vector<time> first_task_end_for_last_job_of_previous_batch(machine_count, 0);
			for (auto batch : solution_to_check.get_planned_batchs())
			{
				for (index machine_index{ 0 }; machine_index < machine_count; ++machine_index)
				{
					if (batch.get_jobs()[0].get_production_date_on_machine(machine_index) < first_task_end_for_last_job_of_previous_batch[machine_index])
						return false;
					first_task_end_for_last_job_of_previous_batch[machine_index] = get_end_of_task(batch.get_jobs().back(), machine_index);
				}
			}
			return true;
		}

		bool check_inventory_costs(solution const & solution_to_check)
		{
			for (auto const & batch : solution_to_check.get_planned_batchs())
			{
				if (batch.get_inventory_cost() != batch.calculate_invetory_cost())
				{
					std::cerr << "invalide batch inventory cost" << std::endl;
					return false;
				}
			}
			cost total_inventory_cost = std::accumulate(solution_to_check.get_planned_batchs().begin(), solution_to_check.get_planned_batchs().end(), 0, [](cost value, planned_batch const & batch) {return value + batch.get_inventory_cost(); });
			if (total_inventory_cost != solution_to_check.get_inventory_cost())
			{
				std::cerr << "invalide inventory cost sum" << std::endl;
				return false;
			}
			return true;
		}
	}

	bool solution_validator::check_solition(solution const & solution_to_check)
	{
		auto const & planned_batchs{ solution_to_check.get_planned_batchs() };
		index const machine_count{ solution_to_check.get_instance()->get_machine_count() };

		// no empty batch allowed
		if (!check_no_empty_batch(solution_to_check)) 
			return false;

		// The first job of a batch wait the end of first task of last job of the previous batch.
		if (!check_no_batchs_production_colision(solution_to_check)) 
			return false;

		for (auto & batch : planned_batchs)
		{
			auto & jobs = batch.get_jobs();
			if (jobs.empty()) continue;


			// only one task at time per machine
			for (index machine_index{ 0 }; machine_index < machine_count; ++machine_index)
			{
				for (index job_index{ 1 }; job_index < jobs.size(); ++job_index)
				{
					if (!is_job_waiting_end_of_machine_previous_task(jobs[job_index - 1], jobs[job_index], machine_index))
					{
						std::cerr << "plusieurs taches simultanés sur une machine" << std::endl;
						return false;
					}
				}
			}

			// only one task at time per job
			for (auto & job : jobs)
			{
				for (index machine_index{ 1 }; machine_index < machine_count; ++machine_index)
				{
					if (!is_machine_waiting_end_of_job_previous_task(job, machine_index - 1, machine_index))
					{
						std::cerr << "plusieurs taches simultanés pour un job" << std::endl;
						return false;
					}
				}
			}
		}
		/*{
			for (auto const & batch : solution_to_check.get_planned_batchs())
			{
				cost inventory_cost = batch.get_inventory_cost();
				cost cound_inventory_cost = batch.calculate_invetory_cost();
				if (inventory_cost != cound_inventory_cost)
				{
					std::cerr << "invalide batch inventory cost, " << batch.get_inventory_cost() << ", " << batch.calculate_invetory_cost() << std::endl;
					return false;
				}
			}
			cost total_inventory_cost = std::accumulate(solution_to_check.get_planned_batchs().begin(), solution_to_check.get_planned_batchs().end(), 0, [](cost value, planned_batch const & batch) {return value + batch.get_inventory_cost(); });
			if (total_inventory_cost != solution_to_check.get_inventory_cost())
			{
				std::cerr << "invalide inventory cost sum" << std::endl;
				return false;
			}
			return true;
		}*/
		/*if (!check_inventory_costs(solution_to_check))
		{
			std::cerr << "invalid inventory cost" << std::endl;
			return false;
		}*/
		return true;
	}
}
