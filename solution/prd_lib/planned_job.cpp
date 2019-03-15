#include "planned_job.h"

namespace solver
{
	planned_job::planned_job(job_ptr job, const std::vector<time>& production_dates, time delivery_date)
		:m_job{ job }, m_production_dates{ production_dates }, m_delivery_date{ delivery_date }
	{
	}

}