#include "solver.h"

namespace solver{
	solution_with_statistics solver::operator()(instance instance_to_solve, resolution_method & method)
	{
		return (*this)(std::make_shared<const_instance_ptr::element_type>(std::move(instance_to_solve)), method);
	}

	solution_with_statistics solver::operator()(const_instance_ptr instance_to_solve, resolution_method & method)
	{
		auto const starting_time = std::chrono::system_clock::now();
		solution const found_solution = method(instance_to_solve);
		auto const end_time = std::chrono::system_clock::now();
		auto const duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - starting_time);
		return solution_with_statistics(found_solution, duration);
	}
}
