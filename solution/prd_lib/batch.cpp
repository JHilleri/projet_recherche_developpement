#include "batch.h"





batch::batch(time_unit first_possible_departure_date, const std::vector<job>& jobs)
	:m_first_possible_departure_date{first_possible_departure_date}, m_jobs{jobs}
{
	// todo : calculate m_last_possible_departure_date
}


time_unit batch::first_possible_departure_date() const
{
	return m_first_possible_departure_date;
}

time_unit batch::last_possible_departure_date() const
{
	return m_last_possible_departure_date;
}

const std::vector<job>& batch::jobs() const
{
	return m_jobs;
}
