#pragma once
#include "solution.h"
#include "linear_function.h"

namespace solver
{
	/// \brief This class hold datas during a the local search resolution batch resolution.
	struct local_search_resolution_data
	{
		solution & result;
		batch const & batch;

		index const batch_to_solve;
		time const departure_window_begining;
		time departure_window_ending;
		Fct_lin delivery_cost_per_departure;
		index machine_count;

		std::vector<time> earliest_production_start;

		local_search_resolution_data(solution & solution_to_work, index batch_to_solve, time departure_window_begining, std::vector<time> const & earliest_production_start);
	};
}

