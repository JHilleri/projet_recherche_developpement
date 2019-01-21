#pragma once
#include <iostream>
#include "instance.h"
#include "solver_types.h"

class instance_reader
{
public:

	instance read(std::istream & input);
};

index_type read_job_index(std::istream & input);


