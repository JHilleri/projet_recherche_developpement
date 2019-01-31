#include "instance.h"

#include <algorithm>

instance::instance()
{
	m_job_number = 0;
	m_machine_number = 0;
}

distance_unit instance::get_distance_between(const job & job_1, const job & job_2) const
{
	return get_distance_between(job_1.get_index(), job_2.get_index());
}

distance_unit instance::get_distance_between(index_type index_job_1, index_type index_job_2) const
{
	return m_distances[distance_index(index_job_1, index_job_2)];
}

index_type instance::get_job_number() const
{
	return m_job_number;
}

index_type instance::get_machine_number() const
{
	return m_machine_number;
}

const std::vector<batch>& instance::get_batchs() const
{
	return m_planned_batchs;
}

std::vector<batch>& instance::get_batchs()
{
	return m_planned_batchs;
}

void instance::set_batchs(const std::vector<batch>& batchs)
{
	m_planned_batchs = batchs;
}

void instance::set_job_number(index_type job_number)
{
	m_job_number = job_number;
}

void instance::set_machine_number(index_type machine_number)
{
	m_machine_number = machine_number;
}

void instance::set_distances(const std::vector<distance_unit>& distances)
{
	m_distances = distances;
}

void instance::set_distance(const job & job_1, const job & job_2, distance_unit distance)
{
	set_distance(job_1.get_index(), job_2.get_index(), distance);
}

void instance::set_distance(index_type index_job_1, index_type index_job_2, distance_unit distance)
{
	m_distances[distance_index(index_job_1, index_job_2)] = distance;
}

index_type instance::distance_index(index_type index_job_1, index_type index_job_2) const
{
	auto[lower_index, greater_index] = std::minmax(index_job_1, index_job_2);
	return greater_index * (greater_index + 1) / 2 + lower_index;
}
