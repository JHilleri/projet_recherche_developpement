#pragma once
#include <vector>
#include <iostream>

class batch_list : public std::vector<std::vector<int>>
{
public:
	using std::vector<std::vector<int>>::vector;
};

std::ostream& operator<<(std::ostream& output, const batch_list &batch_list);