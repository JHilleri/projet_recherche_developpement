#include "exact_method.h"
#include <string>
#include <algorithm>
#include <numeric>

#include "linear_function.h"
#include "branch_and_bound.h"
namespace solver
{
	namespace {
		auto const job_to_index= [](auto &job_ptr)
		{ 
			return job_ptr->get_index(); 
		};

		inline std::pair<time, time> get_departure_window(const_instance_ptr instance_to_solve, index batch_to_solve_index, solution const & result)
		{
			time begining_of_batch_production = 0;
			if (batch_to_solve_index > 0)
			{
				auto const & previous_batch_last_job = result.get_planned_batchs()[batch_to_solve_index - 1].get_jobs().back();
				begining_of_batch_production =  previous_batch_last_job.get_production_date_on_machine(instance_to_solve->get_machine_count());
			}
			batch const & batch_to_solve = instance_to_solve->get_batchs()[batch_to_solve_index];

			time batch_longest_task_duration;
			for (auto const & job : batch_to_solve.get_jobs())
			{
				auto const & durations = job->get_duration_per_machine();
				auto job_longest_task_duration = *std::max_element(durations.begin(), durations.end());
				batch_longest_task_duration = std::max(batch_longest_task_duration, job_longest_task_duration);
			}

			time departure_window_end = begining_of_batch_production + batch_longest_task_duration * (instance_to_solve->get_job_count() + instance_to_solve->get_machine_count() );
			return std::make_pair(begining_of_batch_production, departure_window_end);
		}

		inline void solve_batch(const_instance_ptr instance_to_solve, index batch_to_solve_index, solution & result)
		{
			auto const & jobs = instance_to_solve->get_batchs()[batch_to_solve_index].get_jobs();
			auto const[departure_window_begining, departure_window_end] = get_departure_window(instance_to_solve, batch_to_solve_index, result);
			std::vector<int> jobs_order(jobs.size());
			std::transform(jobs.begin(), jobs.end(), jobs_order.begin(), job_to_index);


			Fct_lin cost_per_departure_date = Branch_and_bound{ *instance_to_solve }.generate_fct_with_branch_and_bound(jobs_order, departure_window_begining, departure_window_end);

			IloEnv cplex_environment;

			size_t segment_count = cost_per_departure_date.tab_link.size();
			//std::vector<cost> segments;

			IloModel cplex_model(cplex_environment);
			IloCplex cplex_solver(cplex_model);

			IloInt machine_count = instance_to_solve->get_machine_count();
			//IloInt job_count = instance_to_solve->get_job_count();
			IloInt job_count = instance_to_solve->get_batchs()[batch_to_solve_index].get_jobs().size();

			IloInt const batch_count = instance_to_solve->get_batchs().size();
			IloInt Nk = instance_to_solve->get_batchs()[0].get_jobs().size();
			//IloArray<IloIntVarArray> P(cplex_environment, m);

			// variables
			IloArray<IloIntVarArray> X(cplex_environment, batch_count); // X_(k, s) vaut 1 si le lots k par dans le segment s
			IloArray<IloIntVarArray> d(cplex_environment, batch_count);
			IloArray<IloIntVarArray> C(cplex_environment, job_count);
			IloArray<IloBoolVarArray> y(cplex_environment, job_count);
			IloIntVarArray J(cplex_environment, batch_count);
			IloIntVarArray F(cplex_environment, batch_count);
			IloFloatVar IC_WIP(cplex_environment);
			IloFloatVar IC_FIN(cplex_environment);
			IloFloatVar IC(cplex_environment);

			// objectif
			//auto objective = IloMinimize(cplex_environment, IC);
			IloExpr objective(cplex_environment);
			objective += IC;

		}
	}
	solution exact_method::operator()(const_instance_ptr instance_to_solve)
	{
		/*
		if (instance_to_solve->get_batchs().size() <= 0) return solution(instance_to_solve);

		IloEnv cplex_environment;

		try
		{
			index segment_count;
			std::vector<cost> segments;

			IloModel cplex_model(cplex_environment);
			IloCplex cplex_solver(cplex_model);

			IloInt machine_count = instance_to_solve->get_machine_count();
			IloInt job_count = instance_to_solve->get_job_count();
			IloInt const batch_count = instance_to_solve->get_batchs().size();
			IloInt Nk = instance_to_solve->get_batchs()[0].get_jobs().size();
			//IloArray<IloIntVarArray> P(cplex_environment, m);

			// variables
			IloArray<IloIntVarArray> X(cplex_environment, batch_count); // X_(k, s) vaut 1 si le lots k par dans le segment s
			IloArray<IloIntVarArray> d(cplex_environment, batch_count);
			IloArray<IloIntVarArray> C(cplex_environment, job_count);
			IloArray<IloBoolVarArray> y(cplex_environment, job_count);
			IloIntVarArray J(cplex_environment, batch_count);
			IloIntVarArray F(cplex_environment, batch_count);
			IloFloatVar IC_WIP(cplex_environment);
			IloFloatVar IC_FIN(cplex_environment);
			IloFloatVar IC(cplex_environment);

			// objectif
			//auto objective = IloMinimize(cplex_environment, IC);
			IloExpr objective(cplex_environment);
			objective += IC;

			for (index index_machine = 0; index_machine < machine_count; ++index_machine)
			{
				for(index index_batch = 0; index_batch < batch_count; ++index_batch )
					for (index index_job = 0; index_job < instance_to_solve->get_batchs()[index_machine].get_jobs().size(); ++index_job)
					{
						auto & is_batch_start_in_segment = X[index_machine][instance_to_solve->get_batchs()[index_batch].get_jobs()[index_job]->get_index()];
						auto & inventory_cost = instance_to_solve->get_batchs()[index_batch].get_jobs()[index_job]->get_in_progress_inventory_cost()[index_machine];
						objective += is_batch_start_in_segment * inventory_cost;
//						objective += d[index_batch][];

					}
			}

			for (index index_batch = 0; index_batch < batch_count; ++index_batch)
			{
				for (index index_segment = 0; index_segment < segment_count; ++index_segment)
				{
					//X[index_batch][index_segment] * segments[index_batch][index_segment];
				}
			}

			for (int i{ 0 }; i < instance_to_solve->get_job_count(); ++i)
			{
				//C[i] = IloIntVarArray(cplex_environment, instance_to_solve->get_machine_count(), 0, INT_MAX); // {0, 1, 2, 3, ..., job_count}
				C[i] = IloIntVarArray(cplex_environment, instance_to_solve->get_machine_count());
			}

		}
		catch (const std::exception& exception)
		{
			std::cerr << exception.what() << std::endl;
		}

		cplex_environment.end(); // todo : make it exception safe
		*/
		return solution(instance_to_solve);
	}
}
