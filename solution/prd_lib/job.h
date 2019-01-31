#pragma once
#include <vector>
#include <memory>

#include "solver_types.h"

class job
{
  public:
	job() = default;
	job(index_type index, const std::vector<time_unit> &duration_per_machine, 
		time_unit due_date, cost_unit penalty_per_delivery_delay, const  std::vector<cost_unit> &in_progress_inventory_cost, cost_unit ended_inventory_cost);

	index_type index() const;
	const std::vector<time_unit> &duration_per_machine() const;
	time_unit total_duration() const;
	time_unit due_date() const;
	cost_unit penalty_per_delivery_delay() const;
	const std::vector<cost_unit> &in_progress_inventory_cost() const;
	cost_unit ended_inventory_cost() const;

	std::vector<time_unit> &duration_per_machine();
	std::vector<cost_unit> &in_progress_inventory_cost();

	void set_index(index_type index);
	void set_duration_per_machine(const std::vector<time_unit> &duration_per_machine);
	void set_due_date(time_unit due_date);
	void set_penalty_per_delivery_delay(cost_unit penalty_per_delivery_delay);
	void set_in_progress_inventory_cost(const std::vector<cost_unit> &in_progress_inventory_cost);
	void set_ended_inventory_cost(cost_unit ended_inventory_cost);

	bool operator==(const job & job2) const;

  private:
	index_type m_index;
	std::vector<time_unit> m_duration_per_machine;
	time_unit m_due_date;
	cost_unit m_penalty_per_delivery;
	std::vector<cost_unit> m_in_progress_inventory_cost;
	cost_unit m_ended_inventory_cost;
};

using job_ptr = std::shared_ptr<job>;
using const_job_ptr = std::shared_ptr<const job>;