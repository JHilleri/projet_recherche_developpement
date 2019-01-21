#include "instance_reader.h"

#include <string>
#include <regex>
#include <algorithm>

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
	std::string index_string{match[1]};
	return std::stoi(index_string);
}

inline instance instance_reader::read(std::istream & input)
{
	index_type job_number;
	index_type machine_number;
	index_type vehicule_cost; // unused

	input >> job_number >> machine_number >> vehicule_cost;
	input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	std::vector<job> jobs{ job_number };

	for (unsigned int job_index{ 0 }; job_index < job_number; ++job_index)
	{
		index_type index{ read_job_index(input) };
		std::vector<time_unit> work_durations(machine_number);
		std::generate(work_durations.begin(), work_durations.end(), [&input]() {
			int work_duration;
			input >> work_duration;
			return work_duration;
		});

	}


	// TODO: Ajoutez ici votre code d'implémentation..
	return instance();
}
