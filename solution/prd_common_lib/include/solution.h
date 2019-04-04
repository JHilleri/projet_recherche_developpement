#pragma once
#include <vector>
#include <memory>
#include <numeric>

#include "solver_types.h"
#include "planned_batch.h"
#include "instance.h"

namespace solver
{
	/// hold datas of a resolved instance
	class solution
	{
	public:
		solution(instance & instance_model) : m_instance{ std::make_shared<instance const>(instance_model) }, m_planned_batchs(instance_model.get_batchs().size()) {}
		solution(const_instance_ptr instance_ptr) : m_instance{ instance_ptr }, m_planned_batchs(instance_ptr->get_batchs().size()) {}

		double get_score() const { return m_score; };
		cost get_inventory_cost() const;
		inline cost get_delivery_cost() const { return m_delivery_cost; }
		inline cost get_total_cost() const { return get_inventory_cost() + get_delivery_cost(); }
		std::vector<planned_batch> const & get_planned_batchs() const { return m_planned_batchs; }
		std::vector<planned_batch> & get_planned_batchs() { return m_planned_batchs; }
		const_instance_ptr get_instance() const { return m_instance; }

		//inline void set_inventory_cost(cost inventory_cost) { m_inventory_cost = inventory_cost; }
		inline void set_delivery_cost(cost delivery_cost) { m_delivery_cost = delivery_cost; }
		inline void set_planned_batchs(const std::vector<planned_batch> &batchs) { m_planned_batchs = batchs; }
		inline void set_score(double score) { m_score = score; }

		inline bool is_valide() const { return m_is_valide; }
		inline void set_is_valid(bool is_valide) { m_is_valide = is_valide; }
	private:
		//cost m_inventory_cost;
		cost m_delivery_cost;
		double m_score;
		std::vector<planned_batch> m_planned_batchs;
		const_instance_ptr m_instance;
		bool m_is_valide = false;
	};
}

