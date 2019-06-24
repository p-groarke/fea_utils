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
#include <string>

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
		if (line.size() > 0 && line.back() == '\r') {
			line.pop_back();
		}
		std::invoke(func, std::move(line));
	}
	return true;
}

inline bool open_text_file(
		const std::filesystem::path& fpath, std::vector<uint8_t>& out) {
	std::ifstream ifs(fpath, std::ios::ate);
	if (!ifs.is_open()) {
		fprintf(stderr, "Couldn't open file : %s\n", fpath.string().c_str());
		return false;
	}

	std::streampos size = ifs.tellg();
	out = {};
	out.reserve(size);
	ifs.seekg(0, std::ios::beg);

	std::string line;
	while (std::getline(ifs, line)) {
		if (line.size() > 0 && line.back() == '\r') {
			line.pop_back();
		}

		for (char c : line) {
			out.push_back(static_cast<uint8_t>(c));
		}
	}
	return true;
}

inline bool open_text_file(const std::filesystem::path& fpath,
		std::vector<std::vector<uint8_t>>& out) {
	out = {};

	return read_text_file(fpath, [&](std::string&& line) {
		out.push_back({});
		for (char c : line) {
			out.back().push_back(static_cast<uint8_t>(c));
		}
	});
}


inline bool open_text_file(
		const std::filesystem::path& fpath, std::string& out) {
	std::vector<uint8_t> data;
	bool ret = open_text_file(fpath, data);

	out = {};
	out.reserve(data.size());
	for (uint8_t u : data) {
		out.push_back(static_cast<char>(u));
	}
	return ret;
}

inline bool open_text_file(
		const std::filesystem::path& fpath, std::vector<std::string>& out) {
	out = {};

	return read_text_file(
			fpath, [&](std::string&& line) { out.push_back(line); });
}

inline bool open_text_file_raw(
		const std::filesystem::path& fpath, std::string& out) {
	std::ifstream ifs(fpath, std::ios::ate);
	if (!ifs.is_open()) {
		fprintf(stderr, "Couldn't open file : %s\n", fpath.string().c_str());
		return false;
	}

	std::streampos size = ifs.tellg();
	out = std::string(size, '\0');
	ifs.seekg(0, std::ios::beg);
	ifs.read(out.data(), size);
	size_t pos = out.find('\0');
	if (pos != std::string::npos) {
		out.resize(pos);
	}
	return true;
}

inline bool open_binary_file(
		const std::filesystem::path& f, std::vector<uint8_t>& out) {
	std::ifstream ifs{ f, std::ios::binary | std::ios::ate };
	if (!ifs.is_open()) {
		fprintf(stderr, "Couldn't open file '%s'\n", f.string().c_str());
		return false;
	}

	std::streampos size = ifs.tellg();
	out = std::vector<uint8_t>(size);
	ifs.seekg(0, std::ios::beg);
	ifs.read(reinterpret_cast<char*>(out.data()), size);
	return true;
}
} // namespace fea
