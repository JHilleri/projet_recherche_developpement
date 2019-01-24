#pragma once

#include "job.h"
#include "solver_types.h"
#include <iostream>

index_type read_job_index(std::istream & input);
job read_job(std::istream & input, index_type machine_number);
std::istream & go_to_next_line(std::istream & input);
