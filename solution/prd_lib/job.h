#pragma once
#include <vector>
#include <memory>

#include "solver_types.h"

namespace solver
{
	class job
	{
	public:
		job() = default;
		job(index index, const std::vector<time> &duration_per_machine,
			time due_date, cost penalty_per_delivery_delay, const  std::vector<cost> &in_progress_inventory_cost, cost ended_inventory_cost);

		index get_index() const;
		const std::vector<time> &get_duration_per_machine() const;
		time get_total_duration() const;
		time get_due_date() const;
		cost get_penalty_per_delivery_delay() const;
		const std::vector<cost> &get_in_progress_inventory_cost() const;
		cost get_ended_inventory_cost() const;

		inline time get_duration_on_machine(index machine_index) const { return m_duration_per_machine[machine_index]; }

		std::vector<time> &get_duration_per_machine();
		std::vector<cost> &get_in_progress_inventory_cost();

		void set_index(index index);
		void set_duration_per_machine(const std::vector<time> &duration_per_machine);
		void set_due_date(time due_date);
		void set_penalty_per_delivery_delay(cost penalty_per_delivery_delay);
		void set_in_progress_inventory_cost(const std::vector<cost> &in_progress_inventory_cost);
		void set_ended_inventory_cost(cost ended_inventory_cost);

		bool operator==(const job & job2) const;

	private:
		index m_index;
		std::vector<time> m_duration_per_machine;
		time m_due_date;
		cost m_penalty_per_delivery;
		std::vector<cost> m_in_progress_inventory_cost;
		cost m_ended_inventory_cost;
	};

	using job_ptr = std::shared_ptr<job>;
	using const_job_ptr = std::shared_ptr<const job>;
}