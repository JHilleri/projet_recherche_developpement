#pragma once

#include <memory>

#include "solver_types.h"
#include "solution.h"

namespace solver
{
	class resolution_method
	{
	public:
		virtual solution operator()(const_instance_ptr instance_to_solve) = 0;
	};
}

