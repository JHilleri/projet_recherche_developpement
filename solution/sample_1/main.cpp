#include "solver.h"
#include "local_search.h"
#include "instance_reader.h"
#include "solution_writer.h"
#include <sstream>
#include <string.h>

int main(int argc, char** args)
{
	if (argc != 2)
	{
		std::cerr << "need the path to the instance file as argument" << std::endl;
	}
	try
	{
		std::string path(args[1]);
		std::ifstream instance_input(path);
		if (!instance_input.is_open())
		{
			throw std::exception(_strdup((std::string("can't open the file ") + path).c_str()));
		}
		solver::solver instance_solver;
		solver::local_search resolution_method;

		solver::instance_reader instance_reader;
		auto instance_to_solve = std::make_shared<solver::instance>(instance_reader.read(instance_input, 5));
		auto solution = instance_solver(instance_to_solve, resolution_method);

		solver::solution_writer writer;

		writer.write(std::cout, solution);
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
	}
	return 0;
}