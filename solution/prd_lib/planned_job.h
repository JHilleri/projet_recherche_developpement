#pragma once
#include "job.h"
#include "solver_types.h"

class planned_job
{
public:
	planned_job() = default;
	planned_job(job_ptr job, const std::vector<time_unit> & production_dates, time_unit delivery_date);

	inline time_unit get_delivery_date() const { return m_delivery_date; }
	inline time_unit get_production_date(index_type machine_index) const { return m_production_dates[machine_index]; }
	inline const_job_ptr get_job() const { return m_job; }
	
	inline void set_production_dates(std::vector<time_unit> & production_dates) { m_production_dates = production_dates; }
	inline void set_job(job_ptr job) { m_job = job; }
	inline void set_delivery_date(time_unit delivery_date) { m_delivery_date = delivery_date; }
private:
	std::vector<time_unit> m_production_dates;
	time_unit m_delivery_date;
	job_ptr m_job;
};

