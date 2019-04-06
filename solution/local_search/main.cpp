#include "solver.h"
#include "local_search.h"
#include "instance_reader.h"
#include "solution_writer.h"
#include "solution_validator.h"
#include "local_search_operators.h"

#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <string.h>

int main(int argc, char** argv)
{
	if (argc != 5)
	{
		std::cerr <<
R"(need the folowing arguments :
- instance file
- job_count_per_batch
- output_file
-  minimum resolution duration in milliseconds
example : instance.txt 10 result.json)" << std::endl;
	}
	try
	{
		std::string path(argv[1]);
		unsigned int job_per_batch = std::stoi(argv[2]);

		std::ofstream output(argv[3], std::ofstream::ios_base::trunc);
		if (!output.is_open())
		{
			throw std::exception(_strdup((std::string("can't open the output file ") + path).c_str()));
		}

		std::ifstream instance_input(path);
		if (!instance_input.is_open())
		{
			throw std::exception(_strdup((std::string("can't open the input file ") + path).c_str()));
		}

		std::chrono::milliseconds minimun_duration(std::stoi(argv[4]));

		solver::instance_reader instance_reader;
		auto instance_to_solve = std::make_shared<solver::instance>(instance_reader.read(instance_input, job_per_batch));

		solver::solver instance_solver;

		//solver::local_search resolution_method(minimun_duration, solver::transposition_neighbourhood_operator, solver::first_beter_neighbor_selector);
		//solver::local_search resolution_method(minimun_duration, solver::transposition_neighbourhood_operator, solver::best_neighbor_selector);
		//solver::local_search resolution_method(minimun_duration, solver::change_job_position_neighbourhood_operator, solver::first_beter_neighbor_selector);
		solver::local_search resolution_method(minimun_duration, solver::change_job_position_neighbourhood_operator, solver::best_neighbor_selector);

		auto solution = instance_solver(instance_to_solve, resolution_method);

		solver::solution_validator validator;
		solution.set_is_valid(validator.check_solition(solution));
		if (!solution.is_valide())
		{
			std::cerr << "invalid found solution" << std::endl;
		}

		solver::solution_writer writer;
		writer.write(output, solution);
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
	}
	return 0;
}