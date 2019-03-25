#include "exact_method.h"
#include <string>
#include <algorithm>
#include <numeric>

#include "linear_function.h"
#include "branch_and_bound.h"
#include "common.h"
namespace solver
{
	namespace {

		void solve_batch(const_instance_ptr instance_to_solve, index batch_to_solve_index, solution & result)
		{
			auto const & jobs = instance_to_solve->get_batchs()[batch_to_solve_index].get_jobs();
			auto const[departure_window_begining, departure_window_end] = get_departure_window(instance_to_solve, batch_to_solve_index, result);
			std::vector<int> jobs_order(jobs.size());
			std::transform(jobs.begin(), jobs.end(), jobs_order.begin(), job_ptr_to_index);


			Branch_and_bound delivery_cost_function_generator(*instance_to_solve);
			Fct_lin cost_per_departure_date = delivery_cost_function_generator.generate_fct_with_branch_and_bound(jobs_order, departure_window_begining, departure_window_end);

			IloEnv cplex_environment;

			size_t segment_count = cost_per_departure_date.tab_link.size();
			//std::vector<cost> segments;

			IloModel cplex_model(cplex_environment);
			IloCplex cplex_solver(cplex_model);

			IloInt machine_count = instance_to_solve->get_machine_count();
			IloInt job_count = jobs.size();

			IloInt const batch_count = instance_to_solve->get_batchs().size();
			//IloInt Nk = instance_to_solve->get_batchs()[0].get_jobs().size();
			//IloArray<IloIntVarArray> P(cplex_environment, m);

			// variables

			// X_(k, s) vaut 1 si le lots k par dans le segment s
			//IloArray<IloIntVarArray> X(cplex_environment, batch_count); 
			// d_(k, s) temps écoulé dans le segment s avant le départ du lot k
			//IloArray<IloIntVarArray> d(cplex_environment, batch_count);
			// C_(i, j) date de fin de la tache de la machine i sur le job j
			IloArray<IloIntVarArray> C(cplex_environment, job_count);
			// Y(j1, j2) vaut 1 si j1 est produit avant j2
			IloArray<IloBoolVarArray> y(cplex_environment, job_count);
			// j dernier job du lot
			IloIntVar J(cplex_environment);
			// F date de fin de production du lot
			IloIntVar F(cplex_environment);
			IloFloatVar IC_WIP(cplex_environment);
			IloFloatVar IC_FIN(cplex_environment);
			IloFloatVar IC(cplex_environment);

			// objectif
			auto objective = IloMinimize(cplex_environment, IC);
			//IloExpr objective(cplex_environment);
			//objective += IC;

			for (int index_job = 0; index_job < job_count; index_job++) {
				C[index_job] = IloIntVarArray(cplex_environment, machine_count, 0, INT_MAX);
				for (int index_machine = 0; index_machine < machine_count; index_machine++) {
					string str = std::string("C_") + std::to_string(index_job) + "_" + std::to_string(index_machine);
					C[index_job][index_machine].setName(str.c_str());
				}
			}

			for (index index_job = 0; index_job < job_count; ++index_job)
			{
				cplex_model.add( C[machine_count-1][index_job] <= F);
			}

			for (index index_job = 1; index_job < job_count; ++index_job)
			{
				for (index index_machine = 0; index_machine < machine_count; ++index_machine)
				{
					cplex_model.add( C[index_machine][index_job - 1] + jobs[index_job]->get_duration_on_machine(index_machine) <= C[index_machine][index_job]);
				}
			}

			for (index index_job = 0; index_job < job_count; ++index_job)
			{
				for (index index_machine = 1; index_machine < machine_count; ++index_machine)
				{
					cplex_model.add(C[index_machine - 1][index_job] + jobs[index_job]->get_duration_on_machine(index_machine) <= C[index_machine][index_job]);
				}
			}

			for (index index_job_1 = 0; index_job_1 < job_count; ++index_job_1)
			{
				cplex_model.add(y[index_job_1][index_job_1] == 0);
				for (index index_job_2 = index_job_1 + 1; index_job_2 < job_count; ++index_job_2)
				{
					cplex_model.add(y[index_job_1][index_job_2] + y[index_job_2][index_job_1] == 1);
				}
			}

			for (index index_machine = 0; index_machine < machine_count; ++index_machine)
			{
				for (index index_job_1 = 0; index_job_1 < job_count; ++index_job_1)
				{

				}
			}

			cplex_model.add(IC_FIN + IC_WIP <= IC);
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
