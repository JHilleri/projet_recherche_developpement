#include "solver.h"
#include "exact_method.h"
#include "instance_reader.h"
#include "solution_writer.h"
#include "solution_validator.h"

#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <string.h>

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		std::cerr <<
			R"(need the folowing arguments :
- instance file
- job_count_per_batch
- output_file
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

		solver::instance_reader instance_reader;
		auto instance_to_solve = std::make_shared<solver::instance>(instance_reader.read(instance_input, job_per_batch));

		solver::solver instance_solver;
		solver::exact_method resolution_method;

		auto solution = instance_solver(instance_to_solve, resolution_method);

		solver::solution_validator validator;
		solution.set_is_valid(validator.check_solition(solution));

		solver::solution_writer writer;
		writer.write(output, solution);
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
	}
	return 0;
}