#include "instance.h"

#include <algorithm>

namespace solver
{
	instance::instance()
	{
		m_machine_number = 0;
	}

	distance instance::get_distance_between(const job & job_1, const job & job_2) const
	{
		return get_distance_between(job_1.get_index(), job_2.get_index());
	}

	distance instance::get_distance_between(index index_job_1, index index_job_2) const
	{
		index distance_location = distance_index(index_job_1, index_job_2);
		return m_distances[distance_location];
	}

	index instance::get_job_count() const
	{
		return m_jobs.size();
	}

	index instance::get_machine_count() const
	{
		return m_machine_number;
	}

	const std::vector<batch>& instance::get_batchs() const
	{
		return m_batchs;
	}

	std::vector<batch>& instance::get_batchs()
	{
		return m_batchs;
	}

	void instance::set_batchs(const std::vector<batch>& batchs)
	{
		m_batchs = batchs;
		index job_count = 0;
		for (batch const & batch : m_batchs)
		{
			job_count += batch.get_jobs().size();
		}
		std::vector<job_ptr> jobs(job_count);
		auto jobs_iterator = jobs.begin();
		for (batch const & batch : m_batchs)
		{
			std::copy(batch.get_jobs().begin(), batch.get_jobs().end(), jobs_iterator);
		}
		std::swap(jobs, m_jobs);
	}

	void instance::set_machine_number(index machine_number)
	{
		m_machine_number = machine_number;
	}

	void instance::set_distances(const std::vector<distance>& distances)
	{
		m_distances = distances;
	}

	void instance::set_distance(const job & job_1, const job & job_2, distance distance)
	{
		set_distance(job_1.get_index(), job_2.get_index(), distance);
	}

	void instance::set_distance(index index_job_1, index index_job_2, distance distance)
	{
		m_distances[distance_index(index_job_1, index_job_2)] = distance;
	}

	const_job_ptr instance::get_job_by_index(index job_index) const
	{
		for (batch const & batch : m_batchs)
		{
			if (auto found = batch.get_job_by_index(job_index); found)
			{
				return found;
			}
		}
		throw std::exception("no job found for this index");
	}


	index instance::distance_index(index index_job_1, index index_job_2) const
	{
		index lower_index = std::min(index_job_1, index_job_2) + 1;
		index greater_index = std::max(index_job_1, index_job_2) + 1;
		index result = greater_index * (greater_index + 1) / 2 - (greater_index - lower_index ) - 1;
		return result;
	}
}
