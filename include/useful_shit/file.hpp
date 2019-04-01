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
#include <filesystem>
#include <fstream>
#include <functional>

namespace fea {

template <class Func>
inline bool read_text_file(
		const std::filesystem::path& fpath, const Func& func) {
	std::ifstream ifs(fpath);
	if (!ifs.is_open()) {
		fprintf(stderr, "Couldn't open file : %s\n", fpath.string().c_str());
		return false;
	}

	std::string line;
	while (std::getline(ifs, line)) {
		if (line.size() > 0) {
			if (line.back() == '\n') {
				line.pop_back();
			}
			if (line.back() == '\r') {
				line.pop_back();
			}
		}
		std::invoke(func, std::move(line));
	}
	return true;
}

inline std::string open_text_file(const std::filesystem::path& fpath) {
	std::ifstream ifs(fpath, std::ios::ate);
	if (!ifs.is_open()) {
		fprintf(stderr, "Couldn't open file : %s\n", fpath.string().c_str());
		return {};
	}

	std::streampos size = ifs.tellg();
	std::string ret(size, '\0');
	ifs.seekg(0, std::ios::beg);
	ifs.read(ret.data(), size);
	size_t pos = ret.find('\0');
	if (pos != std::string::npos) {
		ret.resize(pos);
	}
	return ret;
}

inline std::vector<uint8_t> open_binary_file(const std::filesystem::path& f) {
	std::ifstream ifs{ f, std::ios::binary | std::ios::ate };
	if (!ifs.is_open()) {
		fprintf(stderr, "Couldn't open file '%s'\n", f.string().c_str());
		return {};
	}

	std::streampos size = ifs.tellg();
	std::vector<uint8_t> ret(size);
	ifs.seekg(0, std::ios::beg);
	ifs.read(reinterpret_cast<char*>(ret.data()), size);
	return ret;
}
} // namespace fea
