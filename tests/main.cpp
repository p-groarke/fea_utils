#include <gtest/gtest.h>
#include <string_algorithm.hpp>

namespace {
TEST(str, basics) {
	std::string str = "a string weeee, bang, ding, ow";
	EXPECT_TRUE(str::contains(str, "ding"));
	EXPECT_FALSE(str::contains(str, "dong"));

	std::string caps = "NOT SCREAMING";
	EXPECT_EQ(str::lower_case(caps), "not screaming");
	EXPECT_NE(str::lower_case(caps), "NOT SCREAMING");

	auto vec = str::split(str, ',');
	EXPECT_EQ(vec.size(), 4);

	vec = str::split(str, ", ");
	EXPECT_EQ(vec.size(), 6);

	EXPECT_EQ(str::replace_all(caps, "NOT", "is"), "is SCREAMING");
}
} // namespace

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
