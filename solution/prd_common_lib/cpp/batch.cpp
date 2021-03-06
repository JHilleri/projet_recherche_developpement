#include "batch.h"


#include <algorithm>
#include <numeric>

namespace solver
{
	batch::batch(const job_list & jobs)
		:m_jobs{ jobs }
	{
	}

	batch::batch(std::initializer_list<job_ptr> jobs)
		: m_jobs{ jobs }
	{
	}

	batch::batch(std::initializer_list<job> jobs)
		: m_jobs(jobs.size())
	{
		std::transform(jobs.begin(), jobs.end(), m_jobs.begin(), [](const job & current_job) {return std::make_shared<job>(current_job); });
	}


	const job_list& batch::get_jobs() const
	{
		return m_jobs;
	}

	const_job_ptr batch::get_job_by_index(index job_index) const
	{
		for (auto iterator = m_jobs.begin(); iterator != m_jobs.end(); ++iterator)
		{
			if ((*iterator)->get_index() == job_index)
			{
				return *iterator;
			}
		}
		return { nullptr };
	}
}
