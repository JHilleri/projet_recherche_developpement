#pragma once

#include "resolution_method.h"
//#include "local_search_instance.h"
#include <functional>
#include <chrono>

namespace solver
{
	class batch_solution;
	using neighbourhood_operator = std::function<std::vector<batch_solution>(batch_solution const&)>;
	using neighbor_selector = std::function<std::vector<batch_solution>::iterator(batch_solution const&, std::vector<batch_solution>&)>;

	/// a resolution methode implementing a local search algorithm
	class local_search : public resolution_method
	{
	public:
		local_search(std::chrono::milliseconds minimum_duration, neighbourhood_operator neighbourhood_provider, neighbor_selector neighbourhoud_explorator) 
			: minimum_duration(minimum_duration), m_neighbourhood_provider(neighbourhood_provider), m_neighbourhoud_explorator(neighbourhoud_explorator) {}

		virtual solution operator()(const_instance_ptr instance_to_solve) override;
	private:
		void solve_batch(const_instance_ptr instance_to_solve, index batch_index, solution & output, time earlyer_possible_departure, std::vector<time> const & earliest_production_start);

		neighbourhood_operator m_neighbourhood_provider;
		neighbor_selector m_neighbourhoud_explorator;
		std::chrono::milliseconds minimum_duration;
	};
}

