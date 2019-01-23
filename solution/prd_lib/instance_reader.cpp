#include "instance_reader.h"
#include "instance_reader_utils.h"

#include <string>
#include <regex>
#include <algorithm>



inline instance instance_reader::read(std::istream & input)
{
	index_type job_number;
	index_type machine_number;
	index_type vehicule_cost; // unused
	index_type jobs_per_batch;
	index_type batch_number;
	bool use_multiple_batchs{ false };

	input >> job_number >> machine_number >> vehicule_cost;
	if (use_multiple_batchs)
	{
		input >> jobs_per_batch;
	}
	else
	{
		jobs_per_batch = job_number;
	}
	batch_number = job_number / jobs_per_batch;

	input.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // end of the second line

	time_unit first_possible_departure{ 0 };
	std::vector<batch> batchs(batch_number);
	std::generate(batchs.begin(), batchs.end(), [&]() {
		std::vector<job> jobs(jobs_per_batch);
		std::generate(jobs.begin(), jobs.end(), [&]() {
			return read_job(input, machine_number);
		});
		return batch(jobs);
	});
	instance result;
	result.set_batchs(batchs);


	return result;
}
