#pragma once
#include <vector>

#include <optional>
#include "solver_types.h"
#include "job.h"

namespace solver
{
	using job_list = std::vector<job_ptr>;


	/// This class hold a set of job
	class batch
	{
	public:

		batch(const job_list & jobs);
		batch(const batch &) = default;
		batch() = default;
		batch(std::initializer_list<job_ptr> jobs);
		batch(std::initializer_list<job> jobs);
		batch & operator=(const batch &) = default;

		/*time_unit first_possible_departure_date() const;
		time_unit last_possible_departure_date() const;*/
		const job_list & get_jobs() const;
		const_job_ptr get_job_by_index(index job_index) const;

	private:
		/*time_unit m_first_possible_departure_date;
		time_unit m_last_possible_departure_date;*/
		job_list m_jobs;
	};
}
