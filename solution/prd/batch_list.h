#pragma once
#include <vector>
#include <iostream>

using batch_old = std::vector<int>;

/*
	array of batchs
	each batch is an array of job index
*/
class batch_list : public std::vector<batch_old>
{
public:
	using std::vector<batch_old>::vector;
};

std::ostream& operator<<(std::ostream& output, const batch_list &batch_list);