#pragma once
#include <vector>

#include "solver_types.h"

class job
{
  public:
	job() = default;
	job(index_type index, const std::vector<time_unit> &duration_per_machine, time_unit total_duration,
		time_unit due_date, cost_unit penalty_per_delivery, const  std::vector<cost_unit> &in_progress_inventory_cost, cost_unit ended_inventory_cost);

	inline index_type index() const;
	inline const std::vector<time_unit> &duration_per_machine() const;
	inline time_unit total_duration() const;
	inline time_unit due_date() const;
	inline cost_unit penalty_per_delivery() const;
	inline const std::vector<cost_unit> &in_progress_inventory_cost() const;
	inline cost_unit ended_inventory_cost() const;

	inline std::vector<time_unit> &duration_per_machine();
	inline std::vector<cost_unit> &in_progress_inventory_cost();

	inline void set_index(index_type index);
	inline void set_duration_per_machine(const std::vector<time_unit> &duration_per_machine);
	inline void set_total_duration(time_unit total_duration);
	inline void set_due_date(time_unit due_date);
	inline void set_penalty_per_delivery(cost_unit penalty_per_delivery);
	inline void set_in_progress_inventory_cost(const std::vector<cost_unit> &in_progress_inventory_cost);
	inline void set_ended_inventory_cost(cost_unit ended_inventory_cost);

  private:
	index_type m_index;
	std::vector<time_unit> m_duration_per_machine;
	time_unit m_total_duration;
	time_unit m_due_date;
	cost_unit m_penalty_per_delivery;
	std::vector<cost_unit> m_in_progress_inventory_cost;
	cost_unit m_ended_inventory_cost;
};
