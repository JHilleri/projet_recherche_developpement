#pragma once

#include "../Fct_lin.h"

namespace cost_per_departure_date
{
	class algorithm
	{
	public:
		virtual Fct_lin operator()(Instance & inst, const batch_old & batch_old, int min_a, int max_b) = 0;
	};
}

