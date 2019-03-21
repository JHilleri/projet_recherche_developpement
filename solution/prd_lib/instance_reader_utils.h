#pragma once

#include "job.h"
#include "solver_types.h"
#include <iostream>

namespace solver
{
	/// read an job index from a stream
	index read_job_index(std::istream & input);
	/// read a job from a stream
	job read_job(std::istream & input, index machine_number);
	/// on an input stream, ignore all chars until next line
	std::istream & go_to_next_line(std::istream & input);
}
