#pragma once

#include <ilcplex/ilocplex.h>

#include "resolution_method.h"

namespace solver
{
	/// a resolution methode implementing a local search algorithm
	class local_search : public resolution_method
	{
	public:

		virtual solution operator()(const_instance_ptr instance_to_solve) override;
	private:
		void solve_batch(const_instance_ptr instance_to_solve, index batch_index, solution & output, time earlyer_possible_departure);
	};
}

