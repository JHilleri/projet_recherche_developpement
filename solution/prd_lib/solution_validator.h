#pragma once

#include "solution.h"

namespace solver
{
	/// provide a way to check the validity of a solution
	class solution_validator
	{
	public:
		bool check_solition(solution const & solution_to_check);
	};
}

