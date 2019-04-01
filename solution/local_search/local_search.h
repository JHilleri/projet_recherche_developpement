#pragma once

#include "resolution_method.h"
#include <chrono>

namespace solver
{
	/// a resolution methode implementing a local search algorithm
	class local_search : public resolution_method
	{
	public:
		local_search(std::chrono::milliseconds minimum_duration) : minimum_duration(minimum_duration) {}

		virtual solution operator()(const_instance_ptr instance_to_solve) override;
	private:
		void solve_batch(const_instance_ptr instance_to_solve, index batch_index, solution & output, time earlyer_possible_departure, std::vector<time> const & earliest_production_start);
		std::chrono::milliseconds minimum_duration;
	};
}

