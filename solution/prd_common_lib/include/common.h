#include "linear_function.h"
#include "branch_and_bound.h"
#include "instance.h"
#include "solution.h"
#include "solver_types.h"

namespace solver
{
	std::pair<time, time> get_departure_window(const_instance_ptr instance_to_solve, index batch_to_solve_index, solution const & result);
	time get_last_possible_departure(batch const & batch, time first_depature_date, index machine_count);

	index const job_ptr_to_index(const_job_ptr const & job);
	index const job_to_index(job const & job);

	auto const planned_batch_delivery_cost_accumulator = [](cost partial_sum, planned_batch const & batch) {return partial_sum + batch.get_delivery_cost(); };
	auto const planned_batch_inventory_cost_accumulator = [](cost partial_sum, planned_batch const & batch) {return partial_sum + batch.get_inventory_cost(); };
}