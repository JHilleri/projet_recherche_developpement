#pragma once
#include <iostream>
#include "solution_with_statistics.h"

namespace solver 
{
	/// provide a way to write a solution to an output stream
	class solution_writer
	{
	public:
		void write(std::ostream & output, solution & solution_to_write);
		void write(std::ostream & output, solution_with_statistics & solution_to_write);
	};
}
