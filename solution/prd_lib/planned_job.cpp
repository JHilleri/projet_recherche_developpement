#include "planned_job.h"

planned_job::planned_job(job_ptr job, const std::vector<time_unit>& production_dates, time_unit delivery_date)
	:m_job{job}, m_production_dates{production_dates}, m_delivery_date{delivery_date}
{
}
