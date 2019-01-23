#include "batch.h"


#include <algorithm>
#include <numeric>

batch::batch( const std::vector<job>& jobs)
	:m_jobs{jobs}
{
}


const std::vector<job>& batch::jobs() const
{
	return m_jobs;
}
