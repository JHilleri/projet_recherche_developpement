#pragma once
#include <vector>
#include <memory>

#include "solver_types.h"
#include "planned_batch.h"
#include "instance.h"

class solution
{
public:
	solution(Instance & instance) : m_instance{ new Instance(instance) } {}
	solution(Const_instance_ptr instance_ptr) : m_instance{ instance_ptr } {}

	double score() const { return m_score; };
	inline cost_unit inventory_cost() const { return m_inventory_cost; }
	inline cost_unit delivery_cost() const { return m_delivery_cost; }
	inline cost_unit total_cost() const { return m_inventory_cost + m_delivery_cost; }
	std::vector<planned_batch> const & planned_batchs() const { return m_planned_batchs; }
	Const_instance_ptr instance() const { return m_instance; }

	inline void set_inventory_cost(cost_unit inventory_cost) { m_inventory_cost = inventory_cost; }
	inline void set_delivery_cost(cost_unit delivery_cost) { m_delivery_cost = delivery_cost; }
	inline void set_planned_batchs(const std::vector<planned_batch> &batchs) { m_planned_batchs = batchs; }
	inline void set_score(double score) { m_score = score; }

private:
	cost_unit m_inventory_cost;
	cost_unit m_delivery_cost;
	double m_score;
	std::vector<planned_batch> m_planned_batchs;
	Const_instance_ptr m_instance;
};

