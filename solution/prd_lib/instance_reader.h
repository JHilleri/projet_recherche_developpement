#pragma once
#include <iostream>
#include "instance.h"
#include "solver_types.h"

namespace solver
{
	class instance_reader
	{
	public:

		instance read(std::istream & input, index job_per_batch);

	};
}

