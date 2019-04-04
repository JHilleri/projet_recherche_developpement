#pragma once
#include "local_search.h"
#include "local_search_instance.h"

namespace solver
{
	/// give the list of all transposition available for a given solution
	std::vector<batch_solution> transposition_neighbourhood_operator(batch_solution const & current_solution) {
		auto jobs_count = current_solution.get_jobs().size();
		std::vector<batch_solution> neighbourhood;
		for (index index_1 = 0, neighbor_index = 0; index_1 < jobs_count; ++index_1)
		{
			for (index index_2 = index_1 + 1; index_2 < jobs_count; ++index_2, ++neighbor_index)
			{
				auto & neighbor = neighbourhood.emplace_back(current_solution);
				std::swap(neighbor.get_jobs()[index_1], neighbor.get_jobs()[index_2]);
			}
		}
		return neighbourhood;
	};

	/// give the list of all solution where a job had his position in the production order changed
	std::vector<batch_solution> change_job_position_neighbourhood_operator(batch_solution const & current_solution) {
		auto jobs_count = current_solution.get_jobs().size();
		std::vector<batch_solution> neighbourhood;
		for (index index_1 = 0, neighbor_index = 0; index_1 < jobs_count; ++index_1)
		{
			for (index index_2 = 0; index_2 < jobs_count; ++index_2, ++neighbor_index)
			{
				if (index_1 != index_2)
				{
					auto & neighbor = neighbourhood.emplace_back(current_solution);
					auto & jobs = neighbor.get_jobs();
					auto job_position = jobs.begin() + index_1;
					auto job_destination = jobs.begin() + index_2;
					std::iter_swap(job_position, job_destination);

					if (index_1 < index_2)
					{
						for (auto job_position_next = job_position + 1; job_position_next != job_destination; ++job_position, ++job_position_next)
						{
							std::iter_swap(job_position, job_position_next);
						}
					}
					else // index_1 > index_2 , index_1 can't be equal to index_2 due to the previous if statment
					{
						for (auto job_position_next = job_position - 1; job_position_next != job_destination; --job_position, --job_position_next)
						{
							std::iter_swap(job_position, job_position_next);
						}
					}
					//std::swap(neighbor.get_jobs()[index_1],jobs.begin() + index_1; neighbor.get_jobs()[index_2]);
				}
			}
		}
		return neighbourhood;
	};

	/// explore the solutions and return the first beter solution
	std::vector<batch_solution>::iterator first_beter_neighbor_selector(batch_solution const & current_solution, std::vector<batch_solution> & neighbourhood)
	{
		return std::find_if(neighbourhood.begin(), neighbourhood.end(), [&](batch_solution & neighbor) {
			cost score = neighbor.evaluate_score();
			return (score < current_solution.get_score());
		});
	};

	/// explore all solutions and return the best solution better than the given base solution
	std::vector<batch_solution>::iterator best_neighbor_selector(batch_solution const & current_solution, std::vector<batch_solution> & neighbourhood)
	{
		for (batch_solution & neighbor : neighbourhood)
		{
			neighbor.evaluate_score();
		}
		auto best = std::min_element(neighbourhood.begin(), neighbourhood.end(), [&](batch_solution const & neighbor_1, batch_solution const & neighbor_2) {
			return  neighbor_1.get_score() < neighbor_2.get_score();
			//return (score < current_solution.get_score());
		});
		if (best->get_score() < current_solution.get_score())
		{
			return best;
		}
		else
		{
			return neighbourhood.end();
		}
	};
}