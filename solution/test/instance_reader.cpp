#include "pch.h"
#include "instance_reader.h"
#include "instance.h"

TEST(instance_reader, read_instance_0)
{
	std::istringstream input(
R"(4 10
200000
Jobs 0 :
6 11 6 13 13 2 16 9 7 1 
-5 -1
4 3 2 5 3 6 5 3 4 2
218 100 1 2 3 9
Jobs 1 :
2 5 7 8 5 3 2 1 11 10 
3 -1
5 3 4 7 2 3 5 9 5 6
181 100 1 2 2 9
Jobs 2 :
9 8 8 12 7 6 8 10 15 8 
-4 -1
6 1 3 2 4 4 6 4 1 6
246 100 1 2 3 9
Jobs 3 :
13 3 5 5 7 4 3 8 9 5 
-2 -3
5 8 8 5 5 6 4 4 2 6
234 100 1 2 9 9
Distancier :
5 -4
0	
8	0	
1	7	0
5	7	4	0)");

	instance_reader reader;
	const instance instance = reader.read(input);
	EXPECT_EQ(instance.job_number(), 4);
	EXPECT_EQ(instance.machine_number(), 10);
	EXPECT_EQ(instance.batchs().size(), 1);
	EXPECT_EQ(instance.batchs()[0].jobs().size(), 4);
	batch expected_batch({ 
		{0, {6, 11, 6, 13, 13, 2, 16, 9, 7, 1 }, 218, 3, {4, 3, 2, 5, 3, 6, 5, 3, 4 }, 2 },
		{1, {2, 5, 7, 8, 5, 3, 2, 1, 11, 10 }, 181, 2, {5, 3, 4, 7, 2, 3, 5, 9, 5}, 6 },
		{2, {9, 8, 8, 12, 7, 6, 8, 10, 15, 8 }, 246, 3, {6, 1, 3, 2, 4, 4, 6, 4, 1}, 6 },
		{3, {13, 3, 5, 5, 7, 4, 3, 8, 9, 5}, 234, 9, {5, 8, 8, 5, 5, 6, 4, 4, 2}, 6}
	});
	EXPECT_TRUE(std::equal(instance.batchs()[0].jobs().begin(), instance.batchs()[0].jobs().end(), expected_batch.jobs().begin()));

	EXPECT_EQ(instance.distance_between(0, 0), 0);
	EXPECT_EQ(instance.distance_between(0, 1), 8);
	EXPECT_EQ(instance.distance_between(1, 0), 8);
}