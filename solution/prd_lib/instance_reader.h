#pragma once
#include <iostream>
#include "instance.h"
#include "solver_types.h"

namespace solver
{
	/// provide a way to read an instance from an input stream
	class instance_reader
	{
	public:

		instance read(std::istream & input, index job_per_batch);

	};
}

