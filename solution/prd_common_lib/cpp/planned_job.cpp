#include "planned_job.h"

namespace solver
{
	planned_job::planned_job(const_job_ptr job, const std::vector<time>& production_dates) :
		m_job{ job },
		m_production_dates{ production_dates }
	{

	}
}