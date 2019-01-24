#include "batch_list.h"

std::ostream& operator<<(std::ostream& output, const batch_list &batch_list)
{
	for (const auto& batch_old : batch_list)
	{
		output << "{";
		for (auto i : batch_old)
		{
			output << i << ", ";
		}
		output << "}; ";
	}
	return output;
}