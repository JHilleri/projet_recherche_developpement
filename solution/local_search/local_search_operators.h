#pragma once
#include "local_search.h"
#include "local_search_instance.h"

namespace solver
{
	// jobs_count * (jobs_count - 1)

	std::vector<batch_solution> transposition_neighbourhood_operator(batch_solution const & current_solution) {
		auto jobs_count = current_solution.get_jobs().size();
		std::vector<batch_solution> neighbourhood;
		//std::vector<batch_solution> neighbourhood(((jobs_count - 1) * 2) - 1, current_solution);
		for (index index_1 = 0, neighbor_index = 0; index_1 < jobs_count; ++index_1)
		{
			for (index index_2 = index_1 + 1; index_2 < jobs_count; ++index_2, ++neighbor_index)
			{
				//index neighbor_index = (index_1 * (jobs_count-1)) + (index_2) - 1;
				//auto & neighbor = neighbourhood[neighbor_index];
				auto & neighbor = neighbourhood.emplace_back(current_solution);
				std::swap(neighbor.get_jobs()[index_1], neighbor.get_jobs()[index_2]);
			}
		}

		return neighbourhood;
	};

	std::vector<batch_solution>::iterator first_beter_neighbor_selector(batch_solution const & current_solution, std::vector<batch_solution> & neighbourhood)
	{
		return std::find_if(neighbourhood.begin(), neighbourhood.end(), [&](batch_solution & neighbor) {
			cost score = neighbor.evaluate_score();
			return (score < current_solution.get_score());
		});
	};
}