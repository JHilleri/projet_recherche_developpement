#include "pch.h"
#include "instance_reader.h"
#include "instance.h"
#include <vector>

std::string instance_text()
{
	return 
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
5	7	4	0)";
}

const auto job_prt_comparator = [](const job_ptr & j1, const job_ptr & j2) {
	return *j1 == *j2; 
};

TEST(instance_reader, read_instance_4_job_per_batch)
{
	std::istringstream input(instance_text());

	instance_reader reader;
	const instance instance = reader.read(input, 4);
	EXPECT_EQ(instance.get_job_number(), 4);
	EXPECT_EQ(instance.get_machine_number(), 10);
	EXPECT_EQ(instance.get_batchs().size(), 1);
	EXPECT_EQ(instance.get_batchs()[0].get_jobs().size(), 4);
	batch expected_batch({ 
		{0, {6, 11, 6, 13, 13, 2, 16, 9, 7, 1 }, 218, 3, {4, 3, 2, 5, 3, 6, 5, 3, 4 }, 2 },
		{1, {2, 5, 7, 8, 5, 3, 2, 1, 11, 10 }, 181, 2, {5, 3, 4, 7, 2, 3, 5, 9, 5}, 6 },
		{2, {9, 8, 8, 12, 7, 6, 8, 10, 15, 8 }, 246, 3, {6, 1, 3, 2, 4, 4, 6, 4, 1}, 6 },
		{3, {13, 3, 5, 5, 7, 4, 3, 8, 9, 5}, 234, 9, {5, 8, 8, 5, 5, 6, 4, 4, 2}, 6}
	});
	EXPECT_TRUE(std::equal(instance.get_batchs()[0].get_jobs().begin(), instance.get_batchs()[0].get_jobs().end(), expected_batch.get_jobs().begin(), job_prt_comparator));

	EXPECT_EQ(instance.get_distance_between(0, 0), 0);
	EXPECT_EQ(instance.get_distance_between(0, 1), 8);
	EXPECT_EQ(instance.get_distance_between(1, 0), 8);
}

TEST(instance_reader, read_instance_2_job_per_batch)
{
	std::istringstream input(instance_text());

	instance_reader reader;
	const instance instance = reader.read(input, 2);
	EXPECT_EQ(instance.get_job_number(), 4);
	EXPECT_EQ(instance.get_machine_number(), 10);
	EXPECT_EQ(instance.get_batchs().size(), 2);
	EXPECT_EQ(instance.get_batchs()[0].get_jobs().size(), 2);
	EXPECT_EQ(instance.get_batchs()[1].get_jobs().size(), 2);
	std::vector<batch> expected_batchs
	{ {
		{
			{0, {6, 11, 6, 13, 13, 2, 16, 9, 7, 1 }, 218, 3, {4, 3, 2, 5, 3, 6, 5, 3, 4 }, 2 },
			{1, {2, 5, 7, 8, 5, 3, 2, 1, 11, 10 }, 181, 2, {5, 3, 4, 7, 2, 3, 5, 9, 5}, 6 }
		},
		{
			{2, {9, 8, 8, 12, 7, 6, 8, 10, 15, 8 }, 246, 3, {6, 1, 3, 2, 4, 4, 6, 4, 1}, 6 },
			{3, {13, 3, 5, 5, 7, 4, 3, 8, 9, 5}, 234, 9, {5, 8, 8, 5, 5, 6, 4, 4, 2}, 6}
		}
	} };
	EXPECT_TRUE(std::equal(instance.get_batchs()[0].get_jobs().begin(), instance.get_batchs()[0].get_jobs().end(),expected_batchs[0].get_jobs().begin(), job_prt_comparator));
	EXPECT_TRUE(std::equal(instance.get_batchs()[1].get_jobs().begin(), instance.get_batchs()[1].get_jobs().end(), expected_batchs[1].get_jobs().begin(), job_prt_comparator));

	EXPECT_EQ(instance.get_distance_between(0, 0), 0);
	EXPECT_EQ(instance.get_distance_between(0, 1), 8);
	EXPECT_EQ(instance.get_distance_between(1, 0), 8);
}

TEST(instance_reader, read_instance_3_job_per_batch)
{
	std::istringstream input(instance_text());

	instance_reader reader;
	const instance instance = reader.read(input, 3);
	EXPECT_EQ(instance.get_job_number(), 4);
	EXPECT_EQ(instance.get_machine_number(), 10);
	EXPECT_EQ(instance.get_batchs().size(), 2);
	EXPECT_EQ(instance.get_batchs()[0].get_jobs().size(), 3);
	EXPECT_EQ(instance.get_batchs()[1].get_jobs().size(), 1);
	std::vector<batch> expected_batchs
	{ {
		{
			{0, {6, 11, 6, 13, 13, 2, 16, 9, 7, 1 }, 218, 3, {4, 3, 2, 5, 3, 6, 5, 3, 4 }, 2 },
			{1, {2, 5, 7, 8, 5, 3, 2, 1, 11, 10 }, 181, 2, {5, 3, 4, 7, 2, 3, 5, 9, 5}, 6 },
			{2, {9, 8, 8, 12, 7, 6, 8, 10, 15, 8 }, 246, 3, {6, 1, 3, 2, 4, 4, 6, 4, 1}, 6 }
		},
		{
			{3, {13, 3, 5, 5, 7, 4, 3, 8, 9, 5}, 234, 9, {5, 8, 8, 5, 5, 6, 4, 4, 2}, 6}
		}
	} };
	EXPECT_TRUE(std::equal(instance.get_batchs()[0].get_jobs().begin(), instance.get_batchs()[0].get_jobs().end(), expected_batchs[0].get_jobs().begin(), job_prt_comparator));
	EXPECT_TRUE(std::equal(instance.get_batchs()[1].get_jobs().begin(), instance.get_batchs()[1].get_jobs().end(), expected_batchs[1].get_jobs().begin(), job_prt_comparator));

	EXPECT_EQ(instance.get_distance_between(0, 0), 0);
	EXPECT_EQ(instance.get_distance_between(0, 1), 8);
	EXPECT_EQ(instance.get_distance_between(1, 0), 8);
}
