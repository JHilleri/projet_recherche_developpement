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

void test_job_reading(std::istream & input, index machine_number, const job & expected)
{
	const job readed_job{ read_job(input, machine_number) };

	EXPECT_EQ(readed_job.get_index(), expected.get_index());
	EXPECT_EQ(readed_job.get_due_date(), expected.get_due_date());
	EXPECT_EQ(readed_job.get_ended_inventory_cost(), expected.get_ended_inventory_cost());
	EXPECT_EQ(readed_job.get_penalty_per_delivery_delay(), expected.get_penalty_per_delivery_delay());

	const auto & durations{ readed_job.get_duration_per_machine() };
	const auto & expected_durations{ expected.get_duration_per_machine() };
	EXPECT_EQ(durations.size(), machine_number);
	for (unsigned int i{ 0 }; i < machine_number; ++i)
	{
		EXPECT_EQ(durations[i], expected_durations[i]);
	}

	const auto & costs{ readed_job.get_in_progress_inventory_cost() };
	const auto & expected_costs{ expected.get_in_progress_inventory_cost() };

	EXPECT_EQ(readed_job.get_in_progress_inventory_cost().size(), expected_costs.size());
	for (unsigned int i{ 0 }; i < expected_costs.size(); ++i)
	{
		EXPECT_EQ(readed_job.get_in_progress_inventory_cost()[i], expected_costs[i]);
	}
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
	//test_job_reading(input, machine_number, expected_job);
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
	//test_job_reading(input, machine_number, expected_job);
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
	//test_job_reading(input, machine_number, expected_job);
}

