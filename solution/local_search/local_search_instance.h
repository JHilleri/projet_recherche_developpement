#pragma once

#include <vector>
#include "solution.h"
#include "batch.h"
#include "linear_function.h"
#include "branch_and_bound.h"
#include "local_search.h"

#include <random>

namespace solver
{
	class batch_solution;
	class local_search_instance;
	using permutation = std::pair<index, index>;

	/// \brief hold the datas of a batch solution proposal
	class batch_solution {
	public:
		cost get_score() const;

		cost get_delivery_cost() const;
		cost get_inventory_cost() const;

		std::reference_wrapper<local_search_instance> current_local_search_instance;
		batch_solution(std::vector<std::shared_ptr<job>> const & jobs, std::vector<std::vector<time>> const & delays, local_search_instance & local_search_instance, std::vector<time> const & earliest_production_start);
		batch_solution(batch_solution const & base, permutation permutation_to_perform);


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

	/// \brief This class hold datas during the local search resolution.
	/// do not use an instance of this class on multiple thread.
	class local_search_instance
	{
	private:
		solution & result;
		batch const & batch;

		index const batch_to_solve;
		time const departure_window_begining;
		time departure_window_ending;
		Fct_lin delivery_cost_per_departure;
		index machine_count;
		batch_solution current_solution;

		std::vector<time> earliest_production_start;



	public:
		local_search_instance(solution & solution_to_work, index batch_to_solve, time departure_window_begining, std::vector<time> const & earliest_production_start);
		cost get_current_score() const;
		std::vector<permutation> get_current_permutations() const;
		void permutate(permutation permutation_to_perform);
		cost evaluate_permutation(permutation permutation_to_evaluate);
		batch_solution const & get_current_batch_solution();

		void shuffle();
	private:
		friend batch_solution;
	};
}