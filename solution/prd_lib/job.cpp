#include "job.h"
#include <numeric>

namespace solver
{


	index job::get_index() const
	{
		return m_index;
	}

	const std::vector<time>& job::get_duration_per_machine() const
	{
		return m_duration_per_machine;
	}

	time job::get_total_duration() const
	{
		return std::accumulate(m_duration_per_machine.begin(), m_duration_per_machine.end(), 0);
	}

	time job::get_due_date() const
	{
		return m_due_date;
	}

	cost job::get_penalty_per_delivery_delay() const
	{
		return m_penalty_per_delivery;
	}

	const std::vector<cost>& job::get_in_progress_inventory_cost() const
	{
		return m_in_progress_inventory_cost;
	}

	cost job::get_ended_inventory_cost() const
	{
		return m_ended_inventory_cost;
	}

	std::vector<time>& job::get_duration_per_machine()
	{
		return m_duration_per_machine;
	}

	std::vector<cost>& job::get_in_progress_inventory_cost()
	{
		return m_in_progress_inventory_cost;
	}

	void job::set_index(index index)
	{
		m_index = index;
	}

	void job::set_duration_per_machine(const std::vector<time>& duration_per_machine)
	{
		m_duration_per_machine = duration_per_machine;
	}

	void job::set_due_date(time due_date)
	{
		m_due_date = due_date;
	}

	void job::set_penalty_per_delivery_delay(cost penalty_per_delivery_delay)
	{
		m_penalty_per_delivery = penalty_per_delivery_delay;
	}

	void job::set_in_progress_inventory_cost(const std::vector<cost>& in_progress_inventory_cost)
	{
		m_in_progress_inventory_cost = in_progress_inventory_cost;
	}

	void job::set_ended_inventory_cost(cost ended_inventory_cost)
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
			&& (get_total_duration() == job2.get_total_duration());
	}

	}
