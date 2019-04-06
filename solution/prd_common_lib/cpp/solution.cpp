#include "solution.h"

namespace solver
{
	cost solver::solution::get_inventory_cost() const 
	{
		return std::accumulate(m_planned_batchs.begin(), m_planned_batchs.end(), 0, [](int value, planned_batch const & batch) {return value + batch.get_inventory_cost(); }); 
	}
}
