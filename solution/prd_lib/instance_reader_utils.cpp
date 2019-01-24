#include "instance_reader_utils.h"

#include <string>
#include <regex>
#include <iterator>


index_type read_job_index(std::istream & input)
{
	std::regex job_index_regular_expression("Jobs (\\d+) :");
	std::smatch match;
	std::string job_name;
	std::getline(input, job_name);
	bool job_index_found = std::regex_search(job_name, match, job_index_regular_expression);
	if (!job_index_found)
	{
		throw std::exception("can't find the job index");
	}
	std::string index_string{ match[1] };
	return std::stoi(index_string);
}

std::istream & go_to_next_line(std::istream & input)
{
	return input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

job read_job(std::istream & input, index_type machine_number)
{
	index_type index{ read_job_index(input) };

	std::vector<time_unit> work_durations(machine_number);
	std::copy_n(std::istream_iterator<time_unit>(input), machine_number, work_durations.begin());

	double coord_x, coord_y;						// not used
	input >> coord_x >> coord_y;

	std::vector<cost_unit> in_progress_invetory_cost( machine_number - 1 );
	cost_unit ended_inventory_cost;
	{
		std::istream_iterator<cost_unit> cost_reading_iterator(input);
		std::copy_n( cost_reading_iterator, machine_number - 1, in_progress_invetory_cost.begin());
		ended_inventory_cost = *++cost_reading_iterator;
	}

	time_unit due_date;
	index_type quantity;							// not used
	cost_unit production_cost_per_machine;			// not used
	cost_unit production_cost_ended;				// not used
	cost_unit penalty_per_delivery_delay;
	cost_unit deliverer_penalty_per_delivery_delay;	// not used
	input >> due_date >> quantity >> production_cost_per_machine >> production_cost_ended >> penalty_per_delivery_delay >> deliverer_penalty_per_delivery_delay;

	return job(index, work_durations, due_date, penalty_per_delivery_delay, in_progress_invetory_cost, ended_inventory_cost);
}
