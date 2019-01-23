#include "job.h"
#include <numeric>

job::job(index_type index, const std::vector<time_unit>& duration_per_machine, time_unit due_date,
	cost_unit penalty_per_delivery_delay, const std::vector<cost_unit>& in_progress_inventory_cost, cost_unit ended_inventory_cost) : 
	m_index{index},
	m_duration_per_machine{duration_per_machine},
	m_due_date{due_date},
	m_penalty_per_delivery{penalty_per_delivery_delay},
	m_in_progress_inventory_cost{in_progress_inventory_cost},
	m_ended_inventory_cost{ended_inventory_cost}
{
}

index_type job::index() const
{ 
	return m_index; 
}

const std::vector<time_unit>& job::duration_per_machine() const
{
	return m_duration_per_machine;
}

time_unit job::total_duration() const 
{ 
	return std::accumulate(m_duration_per_machine.begin(), m_duration_per_machine.end(), 0); 
}

time_unit job::due_date() const 
{ 
	return m_due_date; 
}

cost_unit job::penalty_per_delivery_delay() const 
{ 
	return m_penalty_per_delivery; 
}

const std::vector<cost_unit>& job::in_progress_inventory_cost() const 
{ 
	return m_in_progress_inventory_cost; 
}

cost_unit job::ended_inventory_cost() const 
{ 
	return m_ended_inventory_cost; 
}

std::vector<time_unit>& job::duration_per_machine()
{
	return m_duration_per_machine;
}

std::vector<cost_unit>& job::in_progress_inventory_cost()
{
	return m_in_progress_inventory_cost;
}

void job::set_index(index_type index)
{
	m_index = index;
}

void job::set_duration_per_machine(const std::vector<time_unit>& duration_per_machine)
{
	m_duration_per_machine = duration_per_machine;
}

void job::set_due_date(time_unit due_date)
{
	m_due_date = due_date;
}

void job::set_penalty_per_delivery_delay(cost_unit penalty_per_delivery_delay)
{
	m_penalty_per_delivery = penalty_per_delivery_delay;
}

void job::set_in_progress_inventory_cost(const std::vector<cost_unit>& in_progress_inventory_cost)
{
	m_in_progress_inventory_cost = in_progress_inventory_cost;
}

void job::set_ended_inventory_cost(cost_unit ended_inventory_cost)
{
	m_ended_inventory_cost = ended_inventory_cost;
}

bool job::operator==(const job & job2) const
{
	return (m_index == job2.m_index)
		&& (m_due_date == job2.m_due_date)
		&& (m_ended_inventory_cost == job2.m_ended_inventory_cost)
		&& (m_penalty_per_delivery == job2.m_penalty_per_delivery)
		&& (m_duration_per_machine.size() == job2.m_duration_per_machine.size())
		&& (m_in_progress_inventory_cost.size() == job2.m_in_progress_inventory_cost.size())
		&& (std::equal(m_duration_per_machine.begin(), m_duration_per_machine.end(), job2.m_duration_per_machine.begin()))
		&& (std::equal(m_in_progress_inventory_cost.begin(), m_in_progress_inventory_cost.end(), job2.m_in_progress_inventory_cost.begin()))
		&& (total_duration() == job2.total_duration());
}

