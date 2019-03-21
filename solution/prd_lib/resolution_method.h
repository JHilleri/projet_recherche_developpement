#pragma once

#include <memory>

#include "solver_types.h"
#include "solution.h"

namespace solver
{
	/// interface for instance resolution algorithms
	class resolution_method
	{
	public:
		/// solve an instance
		virtual solution operator()(const_instance_ptr instance_to_solve) = 0;
	};
}

