/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2020, Philippe Groarke
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 **/

#pragma once
#include "fea_utils/platform.hpp"
#include "fea_utils/string.hpp"

#include <filesystem>

#include <fstream>
#include <functional>
#include <string>

namespace fea {
// Returns the executable's directory. You must provide argv[0].
inline std::filesystem::path executable_dir(const char* argv0) {
#if defined(FEA_WINDOWS)
	return std::filesystem::absolute(argv0).remove_filename();
#else
	std::filesystem::path c_path = std::filesystem::current_path();
	std::string arg{ argv0 };
	if (starts_with(arg, ".")) {
		arg.erase(0, 1);
	}
	c_path += std::filesystem::path{ arg };
	if (c_path.string().find(".") != std::string::npos) {
		return c_path.remove_filename();
	}

	return c_path.parent_path();
#endif
}


// Returns the full size of the filestream. Leaves the stream at the beginning.
template <class IFStream>
size_t file_size(IFStream& ifs) {
	if (!ifs.is_open()) {
		return 0;
	}

	ifs.seekg(0, ifs.end);
	auto ret = ifs.tellg();
	ifs.seekg(0, ifs.beg);
	return size_t(ret);
}


template <class IFStream, class String, class Func>
bool basic_read_text_file(const std::filesystem::path& fpath, Func func) {
	IFStream ifs(fpath);
	if (!ifs.is_open()) {
		fprintf(stderr, "Couldn't open file : %s\n", fpath.string().c_str());
		return false;
	}

	String line;
	while (std::getline(ifs, line)) {
		if (line.size() > 0 && line.back() == '\r') {
			line.pop_back();
		}
		std::invoke(func, std::move(line));
	}
	return true;
}

// Calls your function for every line in a text file. Removes linefeeds.
// Pass in void(std::string&&)
template <class Func>
bool read_text_file(const std::filesystem::path& fpath, Func func) {
	return basic_read_text_file<std::ifstream, std::string>(fpath, func);
}
// Calls your function for every line in a text file. Removes linefeeds.
// Pass in void(std::wstring&&)
template <class Func>
bool wread_text_file(const std::filesystem::path& fpath, Func func) {
	return basic_read_text_file<std::wifstream, std::wstring>(fpath, func);
}


template <class IFStream, class String, class UInt>
bool basic_open_text_file(
		const std::filesystem::path& fpath, std::vector<UInt>& out) {
	IFStream ifs(fpath, std::ios::ate);
	if (!ifs.is_open()) {
		fprintf(stderr, "Couldn't open file : %s\n", fpath.string().c_str());
		return false;
	}

	out = {};
	out.reserve(file_size(ifs));

	String line;
	while (std::getline(ifs, line)) {
		if (line.size() > 0 && line.back() == '\r') {
			line.pop_back();
		}

		for (auto c : line) {
			out.push_back(static_cast<UInt>(c));
		}
	}
	return true;
}

// Opens the text file as unsigned, and stores it in out.
// The output is a text blob (no linefeeds).
inline bool open_text_file(
		const std::filesystem::path& fpath, std::vector<uint8_t>& out) {
	return basic_open_text_file<std::ifstream, std::string>(fpath, out);
}
// Opens the text file as unsigned, and stores it in out.
// The output is a text blob (no linefeeds).
inline bool wopen_text_file(
		const std::filesystem::path& fpath, std::vector<uint16_t>& out) {
	return basic_open_text_file<std::wifstream, std::wstring>(fpath, out);
}


template <class IFStream, class String, class UInt>
bool basic_open_text_file(const std::filesystem::path& fpath,
		std::vector<std::vector<UInt>>& out) {
	out = {};

	return basic_read_text_file<IFStream, String>(fpath, [&](String&& line) {
		out.push_back({});
		for (auto c : line) {
			out.back().push_back(static_cast<UInt>(c));
		}
	});
}

// Opens text file and files the vector of vector with each line converted to
// unsigned.
inline bool open_text_file(const std::filesystem::path& fpath,
		std::vector<std::vector<uint8_t>>& out) {
	return basic_open_text_file<std::ifstream, std::string>(fpath, out);
}
// Opens text file and files the vector of vector with each line converted to
// unsigned.
inline bool wopen_text_file(const std::filesystem::path& fpath,
		std::vector<std::vector<uint16_t>>& out) {
	return basic_open_text_file<std::wifstream, std::wstring>(fpath, out);
}


template <class IFStream, class UInt, class String>
bool basic_open_text_file(const std::filesystem::path& fpath, String& out) {
	std::vector<UInt> data;
	if (!basic_open_text_file<IFStream, String>(fpath, data)) {
		return false;
	}

	out = {};
	out.reserve(data.size());
	for (UInt u : data) {
		out.push_back(static_cast<typename String::value_type>(u));
	}
	return true;
}

// Opens the text file and stores it in out.
// The output is a text blob (no linefeeds).
inline bool open_text_file(
		const std::filesystem::path& fpath, std::string& out) {
	return basic_open_text_file<std::ifstream, uint8_t>(fpath, out);
}
// Opens the text file and stores it in out.
// The output is a text blob (no linefeeds).
inline bool wopen_text_file(
		const std::filesystem::path& fpath, std::wstring& out) {
	return basic_open_text_file<std::wifstream, uint16_t>(fpath, out);
}


template <class IFStream, class String, class Alloc,
		template <class, class> class Vec>
bool basic_open_text_file(
		const std::filesystem::path& fpath, Vec<String, Alloc>& out) {
	out = {};
	return basic_read_text_file<IFStream, String>(
			fpath, [&](String&& line) { out.push_back(line); });
}

// Opens the text file and each line in the vector.
inline bool open_text_file(
		const std::filesystem::path& fpath, std::vector<std::string>& out) {
	return basic_open_text_file<std::ifstream>(fpath, out);
}
// Opens the text file and each line in the vector.
inline bool wopen_text_file(
		const std::filesystem::path& fpath, std::vector<std::wstring>& out) {
	return basic_open_text_file<std::wifstream>(fpath, out);
}


template <class IFStream, class String>
bool open_text_file_raw(const std::filesystem::path& fpath, String& out) {
	IFStream ifs(fpath, std::ios::ate | std::ios::binary);
	if (!ifs.is_open()) {
		fprintf(stderr, "Couldn't open file : %s\n", fpath.string().c_str());
		return false;
	}

	out = String(file_size(ifs), '\0');
	ifs.read(out.data(), out.size());

	using c_t = typename String::value_type;
	size_t pos = out.find(c_t{}); // The real end is always screwed up.

	if (pos != String::npos) {
		out.resize(pos);
	}
	return true;
}

// Opens the text file as-is (without parsing) and stores it in out.
// Fastest option.
inline bool open_text_file_raw(
		const std::filesystem::path& fpath, std::string& out) {
	return open_text_file_raw<std::ifstream>(fpath, out);
}
// Opens the text file as-is (without parsing) and stores it in out.
// Fastest option.
inline bool wopen_text_file_raw(
		const std::filesystem::path& fpath, std::wstring& out) {
	return open_text_file_raw<std::wifstream>(fpath, out);
}

// Opens binary file and stores bytes in vector.
inline bool open_binary_file(
		const std::filesystem::path& f, std::vector<uint8_t>& out) {
	std::ifstream ifs{ f, std::ios::binary | std::ios::ate };
	if (!ifs.is_open()) {
		fprintf(stderr, "Couldn't open file '%s'\n", f.string().c_str());
		return false;
	}

	out = std::vector<uint8_t>(file_size(ifs));
	ifs.read(reinterpret_cast<char*>(out.data()), out.size());
	return true;
}

template <class IFStream>
inline std::u32string open_text_file_with_bom(IFStream& src) {

	using CharT = typename IFStream::char_type;
	using m_string = std::basic_string<CharT, std::char_traits<CharT>,
			std::allocator<CharT>>;

	enum class encoding {
		utf32be = 0,
		utf32le,
		utf16be,
		utf16le,
		utf8,
		ascii,
	};

	std::vector<m_string> boms = {
		m_string("\x00\x00\xFE\xFF", 4),
		m_string("\xFF\xFE\x00\x00", 4),
		m_string("\xFE\xFF", 2),
		m_string("\xFF\xFE", 2),
		m_string("\xEF\xBB\xBF", 3),
	};


	m_string buffer((std::istreambuf_iterator<CharT>(src)),
			std::istreambuf_iterator<CharT>());

	encoding enc = encoding::ascii;

	for (size_t i = 0; i < boms.size(); ++i) {
		m_string testBom = boms[i];
		if (buffer.compare(0, testBom.size(), testBom) == 0) {
			enc = encoding(i);
			buffer = buffer.substr(testBom.size());
			break;
		}
	}

	switch (enc) {
	case encoding::utf32be: {
		if (buffer.size() % 4 != 0) {
			throw std::logic_error("size in bytes must be a multiple of 4");
		}
		size_t count = buffer.size() / 4;
		std::u32string temp = std::u32string(count, 0);
		for (size_t i = 0; i < count; ++i) {
			temp[i] = static_cast<char32_t>(buffer[i * 4 + 3] << 0
					| buffer[i * 4 + 2] << 8 | buffer[i * 4 + 1] << 16
					| buffer[i * 4 + 0] << 24);
		}
		return temp;
	}
	case encoding::utf32le: {
		if (buffer.size() % 4 != 0) {
			throw std::logic_error("size in bytes must be a multiple of 4");
		}
		size_t count = buffer.size() / 4;
		std::u32string temp = std::u32string(count, 0);
		for (size_t i = 0; i < count; ++i) {
			temp[i] = static_cast<char32_t>(buffer[i * 4 + 0] << 0
					| buffer[i * 4 + 1] << 8 | buffer[i * 4 + 2] << 16
					| buffer[i * 4 + 3] << 24);
		}
		return temp;
	}
	case encoding::utf16be: {
		if (buffer.size() % 2 != 0) {
			throw std::logic_error("size in bytes must be a multiple of 2");
		}
		size_t count = buffer.size() / 2;
		std::u16string temp = std::u16string(count, 0);
		for (size_t i = 0; i < count; ++i) {
			temp[i] = static_cast<char16_t>(
					buffer[i * 2 + 1] << 0 | buffer[i * 2 + 0] << 8);
		}
		return utf16_to_utf32(temp);
	}
	case encoding::utf16le: {
		if (buffer.size() % 2 != 0) {
			throw std::logic_error("size in bytes must be a multiple of 2");
		}
		size_t count = buffer.size() / 2;
		std::u16string temp = std::u16string(count, 0);
		for (size_t i = 0; i < count; ++i) {
			temp[i] = static_cast<char16_t>(
					buffer[i * 2 + 0] << 0 | buffer[i * 2 + 1] << 8);
		}
		return utf16_to_utf32(temp);
	}
	default:
		return any_to_utf32(buffer);
	}
}
} // namespace fea
