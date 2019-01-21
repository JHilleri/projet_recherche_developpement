#include "job.h"


job::job(index_type index, const std::vector<time_unit>& duration_per_machine, time_unit total_duration, time_unit due_date,
	cost_unit penalty_per_delivery, const std::vector<cost_unit>& in_progress_inventory_cost, cost_unit ended_inventory_cost)
	:m_index{index}, m_duration_per_machine{duration_per_machine}, m_total_duration{total_duration}, m_due_date{due_date}, 
	m_penalty_per_delivery{penalty_per_delivery}, m_in_progress_inventory_cost{in_progress_inventory_cost}, m_ended_inventory_cost{ended_inventory_cost}
{
}

inline index_type job::index() const
{ 
	return m_index; 
}

inline const std::vector<time_unit>& job::duration_per_machine() const
{
	return m_duration_per_machine;
}

inline time_unit job::total_duration() const 
{ 
	return m_total_duration; 
}

inline time_unit job::due_date() const 
{ 
	return m_due_date; 
}

inline cost_unit job::penalty_per_delivery() const 
{ 
	return m_penalty_per_delivery; 
}

inline const std::vector<cost_unit>& job::in_progress_inventory_cost() const 
{ 
	return m_in_progress_inventory_cost; 
}

inline cost_unit job::ended_inventory_cost() const 
{ 
	return m_ended_inventory_cost; 
}

inline std::vector<time_unit>& job::duration_per_machine()
{
	return m_duration_per_machine;
}

inline std::vector<cost_unit>& job::in_progress_inventory_cost()
{
	return m_in_progress_inventory_cost;
}

inline void job::set_index(index_type index)
{
	m_index = index;
}

inline void job::set_duration_per_machine(const std::vector<time_unit>& duration_per_machine)
{
	m_duration_per_machine = duration_per_machine;
}

inline void job::set_total_duration(time_unit total_duration)
{
	m_total_duration = total_duration;
}

inline void job::set_due_date(time_unit due_date)
{
	m_due_date = due_date;
}

inline void job::set_penalty_per_delivery(cost_unit penalty_per_delivery)
{
	m_penalty_per_delivery = penalty_per_delivery;
}

inline void job::set_in_progress_inventory_cost(const std::vector<cost_unit>& in_progress_inventory_cost)
{
	m_in_progress_inventory_cost = in_progress_inventory_cost;
}

inline void job::set_ended_inventory_cost(cost_unit ended_inventory_cost)
{
	m_ended_inventory_cost = ended_inventory_cost;
}

