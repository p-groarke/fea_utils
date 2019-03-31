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
#include <sstream>
#include <string>
#include <vector>

namespace str {
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

inline std::string to_lower(const std::string& str) {
	std::string ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(),
			[](char c) { return static_cast<char>(::tolower(c)); });
	return ret;
}

inline std::vector<std::string> split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(str);
	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

inline std::vector<std::string> split(
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

inline std::string replace_all(const std::string& str,
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

} // namespace str
