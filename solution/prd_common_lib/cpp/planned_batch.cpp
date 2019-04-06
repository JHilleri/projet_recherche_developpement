#include "planned_batch.h"
#include <iostream>

namespace solver
{

	planned_batch::planned_batch( cost delivery_cost, const std::vector<planned_job>& jobs)
		: m_delivery_cost{ delivery_cost }, m_jobs{ jobs }
	{
	}

	cost planned_batch::calculate_invetory_cost() const
	{
		if (m_jobs.empty()) return 0;

		cost in_progress_invetory_cost = 0;
		cost ended_inventory_cost = 0;

		index machine_count = m_jobs.front().get_job()->get_duration_per_machine().size();
		for (index index_job = 0; index_job < m_jobs.size(); ++index_job)
		{
			for (index index_machine = 1; index_machine < machine_count; ++index_machine)
			{
					( 
						m_jobs[index_job].get_production_date_on_machine(index_machine) 
						- m_jobs[index_job].get_job()->get_duration_on_machine(index_machine-1)
						- m_jobs[index_job].get_production_date_on_machine(index_machine - 1) 
					) * m_jobs[index_job].get_job()->get_in_progress_inventory_cost()[index_machine-1];
			}
			ended_inventory_cost +=
				(
					m_jobs.back().get_production_date_on_machine(machine_count - 1) + m_jobs.back().get_job()->get_duration_on_machine(machine_count - 1)
					- m_jobs[index_job].get_production_date_on_machine(machine_count - 1) - m_jobs[index_job].get_job()->get_duration_on_machine(machine_count - 1)
				) * m_jobs[index_job].get_job()->get_ended_inventory_cost();
		}

		return in_progress_invetory_cost + ended_inventory_cost;
	}

}