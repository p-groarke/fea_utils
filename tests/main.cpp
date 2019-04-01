#include <gtest/gtest.h>
#include <useful_shit/useful_shit.hpp>

namespace {
TEST(str, basics) {
	std::string str = "a string weeee, bang, ding, ow";
	EXPECT_TRUE(fea::contains(str, "ding"));
	EXPECT_FALSE(fea::contains(str, "dong"));

	std::string caps = "NOT SCREAMING";
	EXPECT_EQ(fea::to_lower(caps), "not screaming");
	EXPECT_NE(fea::to_lower(caps), "NOT SCREAMING");

	auto vec = fea::split(str, ',');
	EXPECT_EQ(vec.size(), 4);

	vec = fea::split(str, ", ");
	EXPECT_EQ(vec.size(), 6);

	EXPECT_EQ(fea::replace_all(caps, "NOT", "is"), "is SCREAMING");
}
} // namespace

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
