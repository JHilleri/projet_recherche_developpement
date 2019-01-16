#pragma once
#include <vector>
#include <iostream>

using batch = std::vector<int>;

/*
	array of batchs
	each batch is an array of job index
*/
class batch_list : public std::vector<batch>
{
public:
	using std::vector<batch>::vector;
};

std::ostream& operator<<(std::ostream& output, const batch_list &batch_list);