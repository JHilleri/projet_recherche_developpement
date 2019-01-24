#pragma once

#include "algorithm.h"
namespace cost_per_departure_date
{

	class edd_near_x4 : public algorithm
	{
	public:
		// Hérité via cost_per_departure_date_algorithm
		virtual Fct_lin operator()(Instance & inst, const batch_old & batch_old, int min_a, int max_b) override;
	};

}