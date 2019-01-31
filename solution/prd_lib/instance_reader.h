#pragma once
#include <iostream>
#include "instance.h"
#include "solver_types.h"

class instance_reader
{
public:

	Instance read(std::istream & input, index_type job_per_batch);

};

