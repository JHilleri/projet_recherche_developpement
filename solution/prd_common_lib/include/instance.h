#pragma once
#include <vector>
#include "solver_types.h"
#include "batch.h"
#include "job.h"

namespace solver
{
	/// This class hold the data of an probleme instance
	class instance
	{
	public:
		instance();
		/*
			return the distance to travel from job_1 delivery location to job_2 delivery location
			[[ expects: job_number() > job_1.index() && job_number() > job_2.index() && job_1.index() >= 0 && job_2.index() >= 0]]
		*/
		distance get_distance_between(const job & job_1, const job & job_2) const;
		distance get_distance_between(index index_job_1, index index_job_2) const ;
		index get_job_count() const;
		index get_machine_count() const;
		const std::vector<batch> & get_batchs() const;

		std::vector<batch> & get_batchs();

		void set_batchs(const std::vector<batch> & batchs);
		void set_machine_number(index machine_number);
		void set_distances(const std::vector<distance> & distances);
		void set_distance(const job & job_1, const job & job_2, distance distance);
		void set_distance(index index_job_1, index index_job_2, distance distance);

		const_job_ptr get_job_by_index(index job_index) const;

		index distance_index(index index_job_1, index index_job_2) const;
	private:
		index m_machine_number;
		std::vector<distance> m_distances;
		std::vector<batch> m_batchs;
		std::vector<job_ptr> m_jobs;

	};

	using const_instance_ptr = std::shared_ptr<instance const>;
}

