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
	distance_unit distance_between(const job & job_1, const job & job_2) const ;
	index_type job_number() const;
	index_type machine_number() const;
	const std::vector<batch> & batchs() const;


	std::vector<batch> & batchs();
	std::vector<std::vector<distance_unit>> & distances();

	void set_batchs(const std::vector<batch> & batchs);
	void set_job_number(index_type job_number);
	void set_machine_number(index_type machine_number);
	void set_distances(const std::vector<std::vector<distance_unit>> & distances);

private:
	index_type m_job_number;
	index_type m_machine_number;
	std::vector<std::vector<distance_unit>> m_distances;
	std::vector<batch> m_batchs;
};

