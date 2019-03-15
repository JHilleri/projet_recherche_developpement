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

	std::optional<const_job_ptr> batch::get_job_by_index(index job_index) const
	{
		auto found_iterator = std::find(m_jobs.begin(), m_jobs.end(), [job_index](job_list::value_type const & job) {return job->get_index() == job_index; });
		if (found_iterator != m_jobs.end()) return *found_iterator;
		return {};
	}
}
