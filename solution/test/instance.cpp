#include "pch.h"
#include "instance.h"


solver::instance &get_test_instance()
{
	static solver::instance test_instance;
	return test_instance;
}

TEST(instance_distance_index, 0) {
	EXPECT_EQ(get_test_instance().distance_index(0, 0), 0u);
}

TEST(instance_distance_index, 1_1) {
	EXPECT_EQ(get_test_instance().distance_index(1, 1), 2u);
}

TEST(instance_distance_index, 1_0) {
	EXPECT_EQ(get_test_instance().distance_index(1, 0), 1u);
}

TEST(instance_distance_index, 2_0) {
	EXPECT_EQ(get_test_instance().distance_index(2, 0), 3u);
}

TEST(instance_distance_index, 9_0) {
	EXPECT_EQ(get_test_instance().distance_index(9, 0), 45u);
}