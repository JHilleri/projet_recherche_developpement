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


			// an arbitrary hight value
			IloInt M = 9999999999;

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
			IloArray<IloIntVarArray> C(cplex_environment, machine_count);

			// Y(j1, j2) vaut 1 si j1 est produit apres j2
			IloArray<IloBoolVarArray> y(cplex_environment, job_count);
			// j dernier job du lot
			IloIntVar J(cplex_environment);

			// F date de fin de production du lot
			IloIntVar F(cplex_environment);

			IloExpr IC_WIP(cplex_environment);

			IloExpr IC_FIN(cplex_environment);

			// objectif
			auto objective = IloMinimize(cplex_environment, IC_WIP + IC_FIN);

			for (index index_job = 0; index_job < job_count; ++index_job)
			{
				y[index_job] = IloBoolVarArray(cplex_environment, job_count);
			}

			for (int index_machine = 0; index_machine < machine_count; index_machine++) {
				C[index_machine] = IloIntVarArray(cplex_environment, job_count, 0, INT_MAX);
				for (int index_job = 0; index_job < job_count; index_job++) {
					string str = std::string("C_") + std::to_string(index_job) + "_" + std::to_string(index_machine);
					C[index_machine][index_job].setName(str.c_str());
				}
			}

			// contrainte debut apres la fin du batche precedent
			if (batch_to_solve_index > 0)
			{
				auto last_job_of_previous_batch =result.get_planned_batchs()[batch_to_solve_index - 1].get_jobs().back();
				for (index  index_job = 0; index_job < job_count; index_job++)
				{
					for (index  index_machine = 0; index_machine < machine_count; index_machine++)
					{
						cplex_model.add( C[index_machine][index_job] >= last_job_of_previous_batch.get_production_date_on_machine(index_machine) + jobs[index_job]->get_duration_on_machine(index_machine));
					}
				}
			}
			else
			{
				for (index index_job = 0; index_job < job_count; ++index_job)
				{
					for (index index_machine = 0; index_machine < machine_count; ++index_machine)
					{
						cplex_model.add(C[index_machine][index_job] >=  jobs[index_job]->get_duration_on_machine(index_machine));
					}
				}
			}

			/*for (index index_job = 0; index_job < job_count; ++index_job)
			{
				cplex_model.add( C[machine_count-1][index_job] <= F);
			}*/

			/*for (index index_job = 1; index_job < job_count; ++index_job)
			{
				for (index index_machine = 0; index_machine < machine_count; ++index_machine)
				{
					cplex_model.add( C[index_machine][index_job - 1] + jobs[index_job]->get_duration_on_machine(index_machine) <= C[index_machine][index_job]);
				}
			}*/

			for (index index_job = 0; index_job < job_count; ++index_job)
			{
				for (index index_machine = 1; index_machine < machine_count; ++index_machine)
				{
					cplex_model.add(C[index_machine][index_job] >= jobs[index_job]->get_duration_on_machine(index_machine) + C[index_machine-1][index_job]);
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

			// contrainte de gamme
			for (index index_machine = 0; index_machine < machine_count; ++index_machine)
			{
				for (index index_job_1 = 0; index_job_1 < job_count; ++index_job_1)
				{
					for (index index_job_2 = 0; index_job_2 < job_count; ++index_job_2)
					{
						if (index_job_1 != index_job_2)
						{
							cplex_model.add(C[index_machine][index_job_2] >= C[index_machine][index_job_1] + jobs[index_job_2]->get_duration_on_machine(index_machine) - (M * y[index_job_1][index_job_2]));
						}
					}
				}
			}

			// contrainte de precedence
			for (index index_machine = 1; index_machine < machine_count; ++index_machine)
			{
				for (index index_job = 0; index_job < job_count; ++index_job)
				{
					cplex_model.add(C[index_machine][index_job] >= C[index_machine-1][index_job] + jobs[index_job]->get_duration_on_machine(index_machine));
				}
			}

			//cplex_model.add(departure_window_begining <= F <= departure_window_end);

			for (index index_job = 0, last_job_index = job_count - 1; index_job < last_job_index; ++index_job)
			{
				IC_FIN += (C[machine_count - 1][last_job_index] - C[machine_count - 1][index_job]) * jobs[index_job]->get_ended_inventory_cost();
			}

			for (index index_machine = 1; index_machine < machine_count; ++index_machine)
			{
				for (index index_job = 0; index_job < job_count; ++index_job)
				{
					IC_WIP += (C[index_machine][index_job] - C[index_machine - 1][index_job] - jobs[index_job]->get_duration_on_machine(index_machine) * jobs[index_job]->get_in_progress_inventory_cost()[index_machine-1]);
				}
			}

			if (!cplex_solver.solve())
			{
				//throw std::exception(_strdup((std::string("batch ") + std::to_string(batch_to_solve_index) + "solver failled").c_str()));
				std::cerr << (std::string("batch ") + std::to_string(batch_to_solve_index) + "solver failled") << std::endl;
			}

			std::vector<std::vector<time>> end_dates(job_count, std::vector<time>(machine_count));
			for (index index_machine = 1; index_machine < machine_count; ++index_machine)
			{
				for (index index_job = 0; index_job < job_count; ++index_job)
				{
					end_dates[index_job][index_machine] = cplex_solver.getValue(C[index_machine][index_job]);
				}
			}

			std::cerr << "y = " << std::endl;
			for (index index_job_1 = 0; index_job_1 < job_count; ++index_job_1)
			{
				for (index index_job_2 = 0; index_job_2 < job_count; ++index_job_2)
				{
					std::cerr << cplex_solver.getValue(y[index_job_1][index_job_2]);
				}
				std::cerr << std::endl;
			}

			std::vector<planned_job> planned_jobs(job_count);
			for (index index_job = 0; index_job < job_count; ++index_job)
			{
				auto dates = end_dates[index_job];
				planned_jobs[index_job].set_production_dates( dates );
				planned_jobs[index_job].set_job(jobs[index_job]);
			}
			std::sort(planned_jobs.begin(), planned_jobs.end(), [](planned_job const & job_1, planned_job const & job_2) {return job_1.get_production_date_on_machine(0) < job_2.get_production_date_on_machine(0); });

			//cplex_model.add(IC_FIN + IC_WIP == IC);
			time production_end = planned_jobs.back().get_production_date_on_machine(machine_count - 1);

			planned_batch batch_result;
			batch_result.set_jobs(planned_jobs);
			batch_result.set_delivery_cost(cost_per_departure_date.eval_fct_lin(production_end));
			result.get_planned_batchs()[batch_to_solve_index] = batch_result;
		}
	}
	solution exact_method::operator()(const_instance_ptr instance_to_solve)
	{
		solution result(instance_to_solve);
		for (index index_batch = 0; index_batch < instance_to_solve->get_batchs().size(); ++index_batch)
		{
			solve_batch(instance_to_solve, index_batch, result);
		}

		cost delivery_cost = std::accumulate(result.get_planned_batchs().begin(), result.get_planned_batchs().end(), 0, planned_batch_delivery_cost_accumulator);
		cost inventory_cost = std::accumulate(result.get_planned_batchs().begin(), result.get_planned_batchs().end(), 0, planned_batch_inventory_cost_accumulator);

		result.set_delivery_cost(delivery_cost);
		result.set_score(delivery_cost + inventory_cost);
		return result;
		
	}
}
