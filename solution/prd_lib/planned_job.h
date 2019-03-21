#pragma once
#include "job.h"
#include "solver_types.h"

namespace solver
{
	class planned_job
	{
	public:
		planned_job() = default;
		//planned_job(job_ptr job, const std::vector<time> & production_dates, time delivery_date);
		planned_job(const_job_ptr job, const std::vector<time> & production_dates);

		//inline time get_delivery_date() const { return m_delivery_date; }
		inline time get_production_date_on_machine(index machine_index) const { return m_production_dates[machine_index]; }
		inline const_job_ptr get_job() const { return m_job; }

		inline void set_production_dates(std::vector<time> & production_dates) { m_production_dates = production_dates; }
		inline void set_job(const_job_ptr job) { m_job = job; }
		//inline void set_delivery_date(time delivery_date) { m_delivery_date = delivery_date; }
	private:
		std::vector<time> m_production_dates;
		//time m_delivery_date;
		const_job_ptr m_job;
	};
}

