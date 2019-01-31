#include "instance_reader.h"
#include "instance_reader_utils.h"

#include <string>
#include <regex>
#include <algorithm>
#include <iterator>
#include <functional>

namespace {
	inline bool start_with(std::string &complet_string, const std::string &start_string)
	{
		return complet_string.size() >= start_string.size() && std::equal(start_string.begin(), start_string.end(), complet_string.begin());
	}

	batch read_batch(std::istream & input, index_type job_count, index_type machine_count)
	{
		job_list jobs(job_count);
		std::generate(jobs.begin(), jobs.end(), [&]() {
			job_ptr result = std::make_shared<job>(read_job(input, machine_count));
			go_to_next_line(input);
			return result;
		});
		return batch(jobs);
	}

	std::vector<batch> read_batchs(std::istream & input, index_type batch_count, index_type job_per_batch, index_type machine_count, index_type job_count)
	{
		std::vector<batch> batchs(batch_count);
		std::generate(batchs.begin(), --batchs.end(), [&]() {return read_batch(input, job_per_batch, machine_count); });
		const index_type last_batch_job_count{ job_count - (batch_count - 1) * job_per_batch};
		batchs.back() = read_batch(input, last_batch_job_count, machine_count);
		return batchs;
	}

	void read_distancier_title(std::istream & input)
	{
		std::string row;
		std::getline(input, row);
		if (!start_with(row, "Distancier :"))
		{
			throw std::exception("invalid input format, 'Distancier :' expected");
		}
	}
}

/*
	read an instance from an input stream
*/
Instance instance_reader::read(std::istream & input, index_type job_per_batch)
{
	// some datas are present in the readed file format but not used in this program
	Instance result;

	index_type job_count;
	index_type machine_count;
	index_type vehicule_cost; // unused
	index_type batch_count;
	input >> job_count >> machine_count >> vehicule_cost;
	
	result.set_job_number(job_count);
	result.set_machine_number(machine_count);

	index_type partial_batch = (job_count % job_per_batch) ? 1 : 0;
	batch_count = (job_count / job_per_batch + partial_batch);

	go_to_next_line(input); // end of the second line

	auto batchs = read_batchs(input, batch_count, job_per_batch, machine_count, job_count);
	result.set_batchs(batchs);

	read_distancier_title(input);

	double deposit_coord_x, deposit_coord_y; // not used
	input >> deposit_coord_x >> deposit_coord_y;

	const index_type distance_count{ (job_count) * (job_count + 1) / 2 };
	std::vector<distance_unit> distances(distance_count);
	std::copy_n(std::istream_iterator<distance_unit>(input), distance_count, distances.begin());
	result.set_distances(distances);

	return result;
}
