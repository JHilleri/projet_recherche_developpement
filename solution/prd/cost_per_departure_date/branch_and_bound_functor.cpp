#include "branch_and_bound_functor.h"
#include "../Branch_and_bound.h"

Fct_lin cost_per_departure_date::branch_and_bound::operator()(Instance & inst, const batch_old & batch_old, int min_a, int max_b)
{
	return Branch_and_bound{ inst }.generate_fct_with_branch_and_bound(batch_old, min_a, max_b);
}
