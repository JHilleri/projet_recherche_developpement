#include "instance.h"



instance::instance()
{
	m_job_number = 0;
	m_machine_number = 0;
}

distance_unit instance::distance_between(const job & job_1, const job & job_2) const
{
	return m_distances[job_1.index()][job_2.index()];
}

index_type instance::job_number() const
{
	return m_job_number;
}

index_type instance::machine_number() const
{
	return m_machine_number;
}

std::vector<std::vector<distance_unit>> & instance::distances()
{
	return m_distances;
}

inline void instance::set_job_number(index_type job_number)
{
	m_job_number = job_number;
}

inline void instance::set_machine_number(index_type machine_number)
{
	m_machine_number = machine_number;
}

inline void instance::set_distances(const std::vector<std::vector<distance_unit>>& distances)
{
	m_distances = distances;
}
