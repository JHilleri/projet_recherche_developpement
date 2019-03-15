#pragma once

#include "job.h"
#include "solver_types.h"
#include <iostream>

namespace solver
{
	index read_job_index(std::istream & input);
	job read_job(std::istream & input, index machine_number);
	std::istream & go_to_next_line(std::istream & input);
}
