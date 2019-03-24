#include "solver.h"
#include "local_search.h"
#include "instance_reader.h"
#include "solution_writer.h"
#include "solution_validator.h"
//#include "exact_method.h"
#include <sstream>
#include <string>
#include <string.h>

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		std::cerr << 
			R"(need the folowing arguments :
			- instance file
			- job_count_per_batch
			- methode_to_use : local_search or exact
			- output_file
			example : prd.exe instance.txt 10 local_search result.json
)" << std::endl;
	}
	try
	{
		std::string path(argv[1]);
		unsigned int job_per_batch = std::stoi(argv[2]);
		std::string methode_to_use(argv[3]);
		std::ofstream output(argv[4], std::ofstream::ios_base::trunc);

		if (!output.is_open())
		{
			throw std::exception(_strdup((std::string("can't open the output file ") + path).c_str()));
		}
		std::unique_ptr<solver::resolution_method> method;
		if (methode_to_use == "local_search")
		{
			method = std::make_unique<solver::local_search>();
		}
		else
		{
			if (methode_to_use == "exact")
			{
				method = std::make_unique<solver::local_search>();

				//method = std::make_unique<solver::exact_method>();
			}
			else
			{
				throw std::exception(_strdup((std::string("invalid resolution methode name :") + methode_to_use).c_str()));
			}
		}


		std::ifstream instance_input(path);
		if (!instance_input.is_open())
		{
			throw std::exception(_strdup((std::string("can't open the input file ") + path).c_str()));
		}
		solver::solver instance_solver;
		solver::local_search resolution_method;

		solver::instance_reader instance_reader;
		auto instance_to_solve = std::make_shared<solver::instance>(instance_reader.read(instance_input, job_per_batch));
		auto solution = instance_solver(instance_to_solve, resolution_method);

		solver::solution_validator validator;
		validator.check_solition(solution);

		solver::solution_writer writer;
		writer.write(output, solution);
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
	}
	return 0;
}