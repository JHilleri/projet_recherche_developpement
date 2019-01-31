#pragma once
#include <vector>

#include "solver_types.h"
#include "batch.h"
#include "planned_job.h"

class planned_batch
{
public:
	planned_batch() = default;
	planned_batch(cost_unit inventory_cost, cost_unit delivery_cost, const std::vector<planned_job> & jobs);

	inline cost_unit get_inventory_cost() const { return m_inventory_cost; }
	inline cost_unit get_delivery_cost() const { return m_delivery_cost; }
	inline cost_unit get_total_cost() const { return m_inventory_cost + m_delivery_cost; }
	inline const std::vector<planned_job> & get_jobs() const { return m_jobs; }

	inline void set_inventory_cost(cost_unit inventory_cost) { m_inventory_cost = inventory_cost; }
	inline void set_delivery_cost(cost_unit delivery_cost) { m_delivery_cost = delivery_cost; }
	inline void set_jobs(const std::vector<planned_job> & jobs) { m_jobs = jobs; }

private:
	cost_unit m_inventory_cost;
	cost_unit m_delivery_cost;
	std::vector<planned_job> m_jobs;
};

