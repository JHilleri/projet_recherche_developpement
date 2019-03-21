#pragma once

#include "resolution_method.h"
#include "instance.h"
#include "solution_with_statistics.h"

namespace solver
{
	/// A class for apply resolution methode on instances and mesure the process duration
	class solver
	{
	public:
		solution_with_statistics operator()(instance instance_to_solve, resolution_method & method);
		solution_with_statistics operator()(const_instance_ptr instance_to_solve, resolution_method & method);
	};
}

