#pragma once
#include <iostream>
#include "instance.h"
#include "solver_types.h"

class instance_reader
{
public:

	instance read(std::istream & input, index_type job_per_batch);

};

