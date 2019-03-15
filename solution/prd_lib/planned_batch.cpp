#include "planned_batch.h"

namespace solver
{

	planned_batch::planned_batch(cost inventory_cost, cost delivery_cost, const std::vector<planned_job>& jobs)
		:m_inventory_cost{ inventory_cost }, m_delivery_cost{ delivery_cost }, m_jobs{ jobs }
	{
	}

}