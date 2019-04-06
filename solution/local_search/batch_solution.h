#pragma once

#include <vector>
#include "batch.h"

#include "local_search_resolution_data.h"
//#include <random>

namespace solver
{
	/// \brief hold the datas of a batch solution proposal
	class batch_solution {
	public:
		cost get_score() const;

		cost get_delivery_cost() const;
		cost get_inventory_cost() const;

		batch_solution(std::shared_ptr<const local_search_resolution_data> local_search_instance, std::vector<time> const & earliest_production_start);

		std::vector<std::shared_ptr<job>> & get_jobs() { return jobs; }
		std::vector<std::vector<time>> & get_delays() { return delays; }

		std::vector<std::shared_ptr<job>> const & get_jobs() const { return jobs; }
		std::vector<std::vector<time>> const & get_delays() const { return delays; }

		std::vector<std::vector<time>> const & get_task_end() const;
		std::vector<std::vector<time>> const & get_task_begin() const;

		/// shuffle the jobs order to make a random job order
		void shuffle();

		cost evaluate_score();
		cost get_score() { return score; }
	private:
		std::shared_ptr<const local_search_resolution_data> resolution_data;
		cost score;

		// list of the batch's jobs, the order in the list is the production order
		std::vector<std::shared_ptr<job>> jobs;

		// [machine][job_position], the delay befor job at job_position on jobs array 
		std::vector<std::vector<time>> delays;

		std::vector<std::vector<time>> tasks_end;
		std::vector<std::vector<time>> tasks_begining;

		std::vector<time> earliest_production_start;

		cost get_total_in_progress_inventory_cost() const;
		cost get_total_ended_inventory_cost() const;
		void update_tasks_dates();
	};
}
