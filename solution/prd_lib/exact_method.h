#pragma once

#include <ilcplex/ilocplex.h>

#include "resolution_method.h"

namespace solver
{
	class exact_method : public resolution_method
	{
	public:

		virtual solution operator()(const_instance_ptr instance_to_solve) override;

	};
}

