﻿#include <cstdio>
#include <cstring>
#include <fea_utils/fea_utils.hpp>
#include <gtest/gtest.h>

namespace {
std::filesystem::path exe_path;

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

TEST(file, basics) {
	std::filesystem::path testfiles_dir = exe_path / "tests_data/";
	for (const std::filesystem::path& filepath :
			std::filesystem::directory_iterator(testfiles_dir)) {
		printf("testing : %s\n", filepath.string().c_str());

		{
			size_t counter = 0;
			fea::read_text_file(filepath, [&](std::string&& line) {
				switch (counter) {
				case 0: {
					EXPECT_EQ(line, "Line1");
				} break;
				case 1: {
					EXPECT_EQ(line, "Line2");
				} break;
				case 2: {
					EXPECT_TRUE(line.empty());
					EXPECT_EQ(line, "");
				} break;
				case 3: {
					EXPECT_EQ(line, "Line4");
				} break;
				default: {
				} break;
				}

				++counter;
			});

			EXPECT_EQ(counter, 4u);
		}

		{
			size_t counter = 0;
			fea::wread_text_file(filepath, [&](std::wstring&& line) {
				switch (counter) {
				case 0: {
					EXPECT_EQ(line, L"Line1");
				} break;
				case 1: {
					EXPECT_EQ(line, L"Line2");
				} break;
				case 2: {
					EXPECT_TRUE(line.empty());
					EXPECT_EQ(line, L"");
				} break;
				case 3: {
					EXPECT_EQ(line, L"Line4");
				} break;
				default: {
				} break;
				}

				++counter;
			});

			EXPECT_EQ(counter, 4u);
		}

		{
			std::vector<uint8_t> text;
			fea::open_text_file(filepath, text);
			EXPECT_FALSE(text.empty());

			const char* testagainst = "Line1Line2Line4";
			std::vector<uint8_t> tester{ testagainst,
				testagainst + strlen(testagainst) };
			EXPECT_EQ(text.size(), tester.size());
			EXPECT_EQ(text, tester);
		}

		{
			std::vector<uint16_t> text;
			fea::wopen_text_file(filepath, text);
			EXPECT_FALSE(text.empty());

			const wchar_t* testagainst = L"Line1Line2Line4";
			std::vector<uint16_t> tester{ testagainst,
				testagainst + std::wcslen(testagainst) };
			EXPECT_EQ(text.size(), tester.size());
			EXPECT_EQ(text, tester);
		}

		{
			std::vector<std::vector<uint8_t>> lines;
			fea::open_text_file(filepath, lines);
			EXPECT_FALSE(lines.empty());

			std::vector<std::vector<uint8_t>> tester;

			const char* line1 = "Line1";
			const char* line2 = "Line2";
			const char* line4 = "Line4";
			tester.push_back({ line1, line1 + strlen(line1) });
			tester.push_back({ line2, line2 + strlen(line2) });
			tester.push_back({});
			tester.push_back({ line4, line4 + strlen(line4) });

			ASSERT_EQ(lines.size(), tester.size());
			for (size_t i = 0; i < lines.size(); ++i) {
				EXPECT_EQ(lines[i].size(), tester[i].size());
				EXPECT_EQ(lines[i], tester[i]);
			}
		}

		{
			std::vector<std::vector<uint16_t>> lines;
			fea::wopen_text_file(filepath, lines);
			EXPECT_FALSE(lines.empty());

			std::vector<std::vector<uint16_t>> tester;

			const wchar_t* line1 = L"Line1";
			const wchar_t* line2 = L"Line2";
			const wchar_t* line4 = L"Line4";
			tester.push_back({ line1, line1 + std::wcslen(line1) });
			tester.push_back({ line2, line2 + std::wcslen(line2) });
			tester.push_back({});
			tester.push_back({ line4, line4 + std::wcslen(line4) });

			ASSERT_EQ(lines.size(), tester.size());
			for (size_t i = 0; i < lines.size(); ++i) {
				EXPECT_EQ(lines[i].size(), tester[i].size());
				EXPECT_EQ(lines[i], tester[i]);
			}
		}

		{
			std::string text;
			fea::open_text_file(filepath, text);
			EXPECT_FALSE(text.empty());

			std::string tester = "Line1Line2Line4";
			EXPECT_EQ(text.size(), tester.size());
			EXPECT_EQ(text, tester);
		}
		{
			std::wstring text;
			fea::wopen_text_file(filepath, text);
			EXPECT_FALSE(text.empty());

			std::wstring tester = L"Line1Line2Line4";
			EXPECT_EQ(text.size(), tester.size());
			EXPECT_EQ(text, tester);
		}

		{
			std::vector<std::string> lines;
			fea::open_text_file(filepath, lines);
			EXPECT_FALSE(lines.empty());

			std::vector<std::string> tester;

			tester.push_back("Line1");
			tester.push_back("Line2");
			tester.push_back("");
			tester.push_back("Line4");

			ASSERT_EQ(lines.size(), tester.size());
			for (size_t i = 0; i < lines.size(); ++i) {
				EXPECT_EQ(lines[i].size(), tester[i].size());
				EXPECT_EQ(lines[i], tester[i]);
			}
		}
		{
			std::vector<std::wstring> lines;
			fea::wopen_text_file(filepath, lines);
			EXPECT_FALSE(lines.empty());

			std::vector<std::wstring> tester;

			tester.push_back(L"Line1");
			tester.push_back(L"Line2");
			tester.push_back(L"");
			tester.push_back(L"Line4");

			ASSERT_EQ(lines.size(), tester.size());
			for (size_t i = 0; i < lines.size(); ++i) {
				EXPECT_EQ(lines[i].size(), tester[i].size());
				EXPECT_EQ(lines[i], tester[i]);
			}
		}

		{
			std::string text;
			fea::open_text_file_raw(filepath, text);
			EXPECT_FALSE(text.empty());

			std::string tester = "Line1\nLine2\n\nLine4";
			if (filepath.string().find("crlf") != std::string::npos) {
				tester = "Line1\r\nLine2\r\n\r\nLine4";
			}
			EXPECT_EQ(text.size(), tester.size());
			EXPECT_EQ(text, tester);
		}
		{
			std::wstring text;
			fea::wopen_text_file_raw(filepath, text);
			EXPECT_FALSE(text.empty());

			std::wstring tester = L"Line1\nLine2\n\nLine4";
			if (filepath.string().find("crlf") != std::string::npos) {
				tester = L"Line1\r\nLine2\r\n\r\nLine4";
			}
			EXPECT_EQ(text.size(), tester.size());
			EXPECT_EQ(text, tester);
		}

		{
			const char* testagainst = "Line1\nLine2\n\nLine4";
			if (filepath.string().find("crlf") != std::string::npos) {
				testagainst = "Line1\r\nLine2\r\n\r\nLine4";
			}

			std::vector<uint8_t> bytes;
			fea::open_binary_file(filepath, bytes);
			EXPECT_FALSE(bytes.empty());

			std::vector<uint8_t> tester{ testagainst,
				testagainst + strlen(testagainst) };
			EXPECT_EQ(bytes.size(), tester.size());
			EXPECT_EQ(bytes, tester);
		}
	}
}
} // namespace

int main(int argc, char** argv) {
	exe_path = fea::executable_dir(argv[0]);

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
