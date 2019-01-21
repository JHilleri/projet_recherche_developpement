#include "pch.h"
#include "solver_types.h"
#include "instance_reader.h"
#include <sstream>

TEST(instance_reader, read_job_index_1_basique) {
	std::istringstream input("Jobs 1 :");

	index_type index = read_job_index(input);

	EXPECT_EQ(index, 1);
}

TEST(instance_reader, read_job_index_1_with_spaces) {
	std::istringstream input("Jobs 1 :   ");

	index_type index = read_job_index(input);

	EXPECT_EQ(index, 1);
}

TEST(instance_reader, read_job_index_42_basique) {
	std::istringstream input("Jobs 42 :");

	index_type index = read_job_index(input);

	EXPECT_EQ(index, 42);
}

TEST(instance_reader, read_job_index_empty_string) {
	std::istringstream input("");

	EXPECT_ANY_THROW(read_job_index(input));
}