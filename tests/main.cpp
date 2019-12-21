#include <fea_utils/fea_utils.hpp>
#include <gtest/gtest.h>

namespace {
TEST(str, basics) {
	std::string str = "a string weeee, bang, ding, ow";
	EXPECT_TRUE(fea::contains(str, "ding"));
	EXPECT_FALSE(fea::contains(str, "dong"));

	std::string caps = "NOT SCREAMING";
	EXPECT_EQ(fea::to_lower(caps), "not screaming");
	EXPECT_NE(fea::to_lower(caps), "NOT SCREAMING");

	std::string capscpy = caps;
	fea::to_lower(capscpy, true);
	EXPECT_EQ(capscpy, "not screaming");
	EXPECT_NE(capscpy, "NOT SCREAMING");

	auto vec = fea::split(str, ',');
	std::vector<std::string> vec_answer{ "a string weeee", " bang", " ding",
		" ow" };
	EXPECT_EQ(vec.size(), 4u);
	EXPECT_EQ(vec, vec_answer);

	vec = fea::split(str, ", ");
	vec_answer = { "a", "string", "weeee", "bang", "ding", "ow" };
	EXPECT_EQ(vec.size(), 6u);
	EXPECT_EQ(vec, vec_answer);

	EXPECT_EQ(fea::replace_all(caps, "NOT", "is"), "is SCREAMING");

	capscpy = caps;
	fea::replace_all(capscpy, "NOT", "is", true);
	EXPECT_EQ(capscpy, "is SCREAMING");
}

TEST(thread, basics) {
	struct my_obj {
		size_t data{ 0 };
	};

	my_obj obj1;
	my_obj obj2;
	my_obj obj3;

	fea::mtx_safe<my_obj> mt_obj{ obj1 };
	fea::mtx_safe<my_obj*> mt_ptr{ &obj2 };
	fea::mtx_safe<my_obj&> mt_ref{ obj3 };

	std::vector<std::function<void()>> funcs;
	for (size_t i = 0; i < 100; ++i) {
		funcs.push_back([&]() { mt_obj.write([](my_obj& o) { ++o.data; }); });
		funcs.push_back([&]() { mt_ptr.write([](my_obj& o) { ++o.data; }); });
		funcs.push_back([&]() { mt_ref.write([](my_obj& o) { ++o.data; }); });
	}
	fea::parallel_tasks(std::move(funcs));

	mt_obj.read([&](const my_obj& o) { EXPECT_EQ(o.data, 100u); });
	mt_ptr.read([&](const my_obj& o) { EXPECT_EQ(o.data, 100u); });
	mt_ref.read([&](const my_obj& o) { EXPECT_EQ(o.data, 100u); });
}

TEST(scope, basics) {
	size_t test_var = 0;

	{
		fea::on_exit e{ [&]() { ++test_var; } };
	}
	EXPECT_EQ(test_var, 1u);
}
} // namespace

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
