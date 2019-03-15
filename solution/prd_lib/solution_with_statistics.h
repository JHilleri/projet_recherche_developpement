#pragma once

#include <chrono>
#include "solution.h"

namespace solver
{
	class solution_with_statistics : public solution
	{
	private:
		std::chrono::milliseconds m_process_duration;
	public:
		solution_with_statistics(solution && base_solution, decltype(m_process_duration) const & duration) : solution(base_solution), m_process_duration(duration) {}
		solution_with_statistics(solution const & base_solution, decltype(m_process_duration) const & duration) : solution(base_solution), m_process_duration(duration) {}

		inline decltype(m_process_duration) const & get_process_duration() const { return m_process_duration; }
		inline void set_process_duration(decltype(m_process_duration) const & duration) { m_process_duration = duration; }
	};
}

