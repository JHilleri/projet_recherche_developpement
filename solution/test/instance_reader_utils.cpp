#include "pch.h"
#include "solver_types.h"
#include "instance_reader_utils.h"
#include "job.h"
#include <sstream>
#include <string>
#include <vector>

using namespace solver;

TEST(instance_reader_utils, read_job_index_1_basique) {
	std::istringstream input("Jobs 1 :");

	index index = read_job_index(input);

	EXPECT_EQ(index, 1);
}

TEST(instance_reader_utils, read_job_index_1_with_spaces) {
	std::istringstream input("Jobs 1 :   ");

	index index = read_job_index(input);

	EXPECT_EQ(index, 1);
}

TEST(instance_reader_utils, read_job_index_42_basique) {
	std::istringstream input("Jobs 42 :");

	index index = read_job_index(input);

	EXPECT_EQ(index, 42);
}

TEST(instance_reader_utils, read_job_index_empty_string) {
	std::istringstream input("");

	EXPECT_ANY_THROW(read_job_index(input));
}

TEST(instance_reader_utils, read_job)
{
	std::istringstream input(
		R"(Jobs 0 :
			10 1 5 6
			12 15.5
			1 1 2 1
			20 100 1 2 2 9)"
	);
	job expected_job;
	const unsigned int machine_number{ 4 };
	expected_job.set_index(0);
	expected_job.set_duration_per_machine({ 10, 1, 5, 6 });
	expected_job.set_in_progress_inventory_cost({ 1, 1, 2 });
	expected_job.set_due_date(20);
	expected_job.set_ended_inventory_cost(1);
	expected_job.set_penalty_per_delivery_delay(2);

	const job readed_job{ read_job(input, machine_number) };

	EXPECT_EQ(readed_job, expected_job);
}

TEST(instance_reader_utils, read_job_42)
{
	std::istringstream input(
		R"(Jobs 42 :
			10 1 5 6
			12 15.5
			1 1 2 1
			20 100 1 2 2 9)"
	);
	job expected_job;
	const unsigned int machine_number{ 4 };
	expected_job.set_index(42);
	expected_job.set_duration_per_machine({ 10, 1, 5, 6 });
	expected_job.set_in_progress_inventory_cost({ 1, 1, 2 });
	expected_job.set_due_date(20);
	expected_job.set_ended_inventory_cost(1);
	expected_job.set_penalty_per_delivery_delay(2);

	const job readed_job{ read_job(input, machine_number) };
	EXPECT_EQ(readed_job, expected_job);
}

TEST(instance_reader_utils, read_bad_job)
{
	std::istringstream input(
		R"(Jobs 0 :
			10 1 5 6
			12 15.5
			1 1 2 1
			20 100 1 2 2 9)"
	);
	job expected_job;
	const unsigned int machine_number{ 4 };
	expected_job.set_index(42);
	expected_job.set_duration_per_machine({ 10, 1, 5, 6 });
	expected_job.set_in_progress_inventory_cost({ 1, 1, 2 });
	expected_job.set_due_date(20);
	expected_job.set_ended_inventory_cost(1);
	expected_job.set_penalty_per_delivery_delay(2);

	const job readed_job{ read_job(input, machine_number) };
	EXPECT_FALSE(readed_job == expected_job);
}

