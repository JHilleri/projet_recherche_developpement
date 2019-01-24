#pragma once
#include <vector>
#include "solver_types.h"
#include "batch.h"
#include "job.h"

class instance
{
public:
	instance();
	/*
		return the distance to travel from job_1 delivery location to job_2 delivery location
		[[ expects: job_number() > job_1.index() && job_number() > job_2.index() && job_1.index() >= 0 && job_2.index() >= 0]]
	*/
	distance_unit distance_between(const job & job_1, const job & job_2) const;
	distance_unit distance_between(index_type index_job_1, index_type index_job_2) const ;
	index_type job_number() const;
	index_type machine_number() const;
	const std::vector<batch> & batchs() const;


	std::vector<batch> & batchs();

	void set_batchs(const std::vector<batch> & batchs);
	void set_job_number(index_type job_number);
	void set_machine_number(index_type machine_number);
	void set_distances(const std::vector<distance_unit> & distances);
	void set_distance(const job & job_1, const job & job_2, distance_unit distance);
	void set_distance(index_type index_job_1, index_type index_job_2, distance_unit distance);

private:
	index_type m_job_number;
	index_type m_machine_number;
	std::vector<distance_unit> m_distances;
	std::vector<batch> m_batchs;

	index_type distance_index(index_type index_job_1, index_type index_job_2) const;
};

