#pragma once
#include <vector>

#include "solver_types.h"
#include "job.h"

/*
	This class represent only the instance data for a batch, not the resolved solution.
*/
class batch
{
	// This class have no attribut setters as some value are calculated from all the others.
	// to change the values, replace the object with a new one.
public:

	batch(time_unit first_possible_departure_date, const std::vector<job> & jobs);
	batch(const batch &) = default;
	batch & operator=(const batch &) = default;

	time_unit first_possible_departure_date() const;
	time_unit last_possible_departure_date() const;
	const std::vector<job> & jobs() const;

private:
	time_unit m_first_possible_departure_date;
	time_unit m_last_possible_departure_date;
	std::vector<job> m_jobs;
};
