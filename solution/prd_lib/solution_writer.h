#pragma once
#include <iostream>
#include "solution_with_statistics.h"

namespace solver 
{
	class solution_writer
	{
	public:
		void write(std::ostream & output, solution & solution_to_write);
	};
}
