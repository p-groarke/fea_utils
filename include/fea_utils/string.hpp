/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2019, Philippe Groarke
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
#include <algorithm>
#include <codecvt>
#include <sstream>
#include <string>
#include <vector>

namespace fea {
template <class T>
inline bool contains(const std::string& str, T search) {
	return str.find(search) != std::string::npos;
}

inline bool starts_with(const std::string& str, const std::string& search) {
	return str.find(search) == 0;
}

inline bool ends_with(const std::string& str, const std::string& search) {
	return str.find(search) == str.size() - search.size();
}

[[nodiscard]] inline std::string to_lower(const std::string& str) {
	std::string ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(),
			[](char c) { return static_cast<char>(::tolower(c)); });
	return ret;
}

inline void to_lower(std::string& out, bool /*inplace*/) {
	std::transform(out.begin(), out.end(), out.begin(),
			[](char c) { return static_cast<char>(::tolower(c)); });
}

[[nodiscard]] inline std::vector<uint8_t> to_lower(
		const std::vector<uint8_t>& str) {
	std::vector<uint8_t> ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(),
			[](char c) { return static_cast<char>(::tolower(c)); });
	return ret;
}

inline void to_lower(std::vector<uint8_t>& out, bool /*inplace*/) {
	std::transform(out.begin(), out.end(), out.begin(),
			[](char c) { return static_cast<char>(::tolower(c)); });
}

[[nodiscard]] inline std::vector<std::string> split(
		const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(str);
	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

[[nodiscard]] inline std::vector<std::string> split(
		const std::string& str, const char* delimiters) {
	std::vector<std::string> tokens;
	size_t prev = 0;
	size_t pos;

	while ((pos = str.find_first_of(delimiters, prev)) != std::string::npos) {
		if (pos > prev) {
			tokens.push_back(str.substr(prev, pos - prev));
		}
		prev = pos + 1;
	}
	if (prev < str.length()) {
		tokens.push_back(str.substr(prev, std::string::npos));
	}
	return tokens;
}

[[nodiscard]] inline std::string replace_all(const std::string& str,
		const std::string& search, const std::string& replace) {

	std::string ret = str;
	size_t pos = ret.find(search);
	while (pos != std::string::npos) {
		ret.replace(pos, search.size(), replace);
		--pos;
		pos = ret.find(search, pos + search.size());
	}
	return ret;
}

inline void replace_all(std::string& out, const std::string& search,
		const std::string& replace, bool /*inplace*/) {

	size_t pos = out.find(search);
	while (pos != std::string::npos) {
		out.replace(pos, search.size(), replace);
		--pos;
		pos = out.find(search, pos + search.size());
	}
}

// The fucking standard doesn't provide codecvt equivalents. Use the old
// functionality until they do.
#pragma warning(push)
#pragma warning(disable : 4996)

// @Brent
// https://stackoverflow.com/questions/38688417/utf-conversion-functions-in-c11
inline std::string to_utf8(const std::u16string& s) {
	std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> conv;
	return conv.to_bytes(s);
}

inline std::string to_utf8(const std::u32string& s) {
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
	return conv.to_bytes(s);
}

inline std::u16string to_utf16(const std::string& s) {
	std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> convert;
	return convert.from_bytes(s);
}

inline std::u32string to_utf32(const std::string& s) {
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
	return conv.from_bytes(s);
}

inline std::u16string to_utf16(const std::u32string& s) {
	return to_utf16(to_utf8(s));
}

inline std::u32string to_utf32(const std::u16string& s) {
	return to_utf32(to_utf8(s));
}

inline std::u32string read_with_bom(std::istream& src) {

	enum encoding {
		encoding_utf32be = 0,
		encoding_utf32le,
		encoding_utf16be,
		encoding_utf16le,
		encoding_utf8,
		encoding_ascii,
	};

	std::vector<std::string> boms = {
		std::string("\x00\x00\xFE\xFF", 4),
		std::string("\xFF\xFE\x00\x00", 4),
		std::string("\xFE\xFF", 2),
		std::string("\xFF\xFE", 2),
		std::string("\xEF\xBB\xBF", 3),
	};

	std::string buffer((std::istreambuf_iterator<char>(src)),
			std::istreambuf_iterator<char>());

	encoding enc = encoding_ascii;

	for (size_t i = 0; i < boms.size(); ++i) {
		std::string testBom = boms[i];
		if (buffer.compare(0, testBom.length(), testBom) == 0) {
			enc = encoding(i);
			buffer = buffer.substr(testBom.length());
			break;
		}
	}

	switch (enc) {
	case encoding_utf32be: {
		if (buffer.length() % 4 != 0) {
			throw std::logic_error("size in bytes must be a multiple of 4");
		}
		size_t count = buffer.length() / 4;
		std::u32string temp = std::u32string(count, 0);
		for (size_t i = 0; i < count; ++i) {
			temp[i] = static_cast<char32_t>(buffer[i * 4 + 3] << 0
					| buffer[i * 4 + 2] << 8 | buffer[i * 4 + 1] << 16
					| buffer[i * 4 + 0] << 24);
		}
		return temp;
	}
	case encoding_utf32le: {
		if (buffer.length() % 4 != 0) {
			throw std::logic_error("size in bytes must be a multiple of 4");
		}
		size_t count = buffer.length() / 4;
		std::u32string temp = std::u32string(count, 0);
		for (size_t i = 0; i < count; ++i) {
			temp[i] = static_cast<char32_t>(buffer[i * 4 + 0] << 0
					| buffer[i * 4 + 1] << 8 | buffer[i * 4 + 2] << 16
					| buffer[i * 4 + 3] << 24);
		}
		return temp;
	}
	case encoding_utf16be: {
		if (buffer.length() % 2 != 0) {
			throw std::logic_error("size in bytes must be a multiple of 2");
		}
		size_t count = buffer.length() / 2;
		std::u16string temp = std::u16string(count, 0);
		for (size_t i = 0; i < count; ++i) {
			temp[i] = static_cast<char16_t>(
					buffer[i * 2 + 1] << 0 | buffer[i * 2 + 0] << 8);
		}
		return to_utf32(temp);
	}
	case encoding_utf16le: {
		if (buffer.length() % 2 != 0) {
			throw std::logic_error("size in bytes must be a multiple of 2");
		}
		size_t count = buffer.length() / 2;
		std::u16string temp = std::u16string(count, 0);
		for (size_t i = 0; i < count; ++i) {
			temp[i] = static_cast<char16_t>(
					buffer[i * 2 + 0] << 0 | buffer[i * 2 + 1] << 8);
		}
		return to_utf32(temp);
	}
	default:
		return to_utf32(buffer);
	}
}
#pragma warning(pop)
} // namespace fea
