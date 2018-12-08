#include "batch_list.h"

std::ostream& operator<<(std::ostream& output, const batch_list &batch_list)
{
	for (const auto& batch : batch_list)
	{
		output << "{";
		for (auto i : batch)
		{
			output << i << ", ";
		}
		output << "}; ";
	}
	return output;
}