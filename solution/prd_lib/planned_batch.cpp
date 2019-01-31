#include "planned_batch.h"


planned_batch::planned_batch(cost_unit inventory_cost, cost_unit delivery_cost, const std::vector<planned_job>& jobs)
	:m_inventory_cost{inventory_cost}, m_delivery_cost{delivery_cost}, m_jobs{jobs}
{
}
