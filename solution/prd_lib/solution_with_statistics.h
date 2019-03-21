#pragma once

#include <chrono>
#include "solution.h"

namespace solver
{
	class solution_with_statistics : public solution
	{
	private:
		using duratuion_type = std::chrono::milliseconds;
		duratuion_type m_process_duration;
	public:
		solution_with_statistics(solution && base_solution, duratuion_type const & duration) : solution(base_solution), m_process_duration(duration) {}
		solution_with_statistics(solution const & base_solution, duratuion_type const & duration) : solution(base_solution), m_process_duration(duration) {}

		inline duratuion_type const & get_process_duration() const { return m_process_duration; }
		inline void set_process_duration(duratuion_type const & duration) { m_process_duration = duration; }
	};
}

