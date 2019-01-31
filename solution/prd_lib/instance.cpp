#include "instance.h"

#include <algorithm>

Instance::Instance()
{
	m_job_number = 0;
	m_machine_number = 0;
}

distance_unit Instance::distance_between(const job & job_1, const job & job_2) const
{
	return distance_between(job_1.index(), job_2.index());
}

distance_unit Instance::distance_between(index_type index_job_1, index_type index_job_2) const
{
	return m_distances[distance_index(index_job_1, index_job_2)];
}

index_type Instance::job_number() const
{
	return m_job_number;
}

index_type Instance::machine_number() const
{
	return m_machine_number;
}

const std::vector<batch>& Instance::batchs() const
{
	return m_planned_batchs;
}

std::vector<batch>& Instance::batchs()
{
	return m_planned_batchs;
}

void Instance::set_batchs(const std::vector<batch>& batchs)
{
	m_planned_batchs = batchs;
}

void Instance::set_job_number(index_type job_number)
{
	m_job_number = job_number;
}

void Instance::set_machine_number(index_type machine_number)
{
	m_machine_number = machine_number;
}

void Instance::set_distances(const std::vector<distance_unit>& distances)
{
	m_distances = distances;
}

void Instance::set_distance(const job & job_1, const job & job_2, distance_unit distance)
{
	set_distance(job_1.index(), job_2.index(), distance);
}

void Instance::set_distance(index_type index_job_1, index_type index_job_2, distance_unit distance)
{
	m_distances[distance_index(index_job_1, index_job_2)] = distance;
}

index_type Instance::distance_index(index_type index_job_1, index_type index_job_2) const
{
	auto[lower_index, greater_index] = std::minmax(index_job_1, index_job_2);
	return greater_index * (greater_index + 1) / 2 + lower_index;
}
