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
#include <chrono>
#include <thread>

namespace fea {
inline size_t num_threads() {
	size_t concurrency = std::thread::hardware_concurrency();
	return concurrency <= 0 ? 1 : concurrency;
}

// Roll our own parallel_for due to build issues with tbb and c++17.
inline void parallel_for(size_t loop_count,
		const std::function<void(const std::pair<size_t, size_t>&, size_t)>&
				func) {

	const size_t num_t = num_threads();

	std::vector<std::pair<size_t, size_t>> index_ranges(num_t, { 0, 0 });
	size_t chunk_size = loop_count / num_t;
	size_t chunk_extra = loop_count % num_t;
	for (size_t i = 0; i < num_t; ++i) {
		index_ranges[i].first = i * chunk_size;
		index_ranges[i].second = (i * chunk_size) + chunk_size;
		if (i == num_t - 1) {
			index_ranges[i].second += chunk_extra;
		}
	}

	std::vector<std::thread> threads;
	threads.reserve(num_t);
	for (size_t i = 0; i < num_t; ++i) {
		threads.emplace_back(func, index_ranges[i], i);
	}

	for (auto& t : threads) {
		t.join();
	}
}

inline void parallel_tasks(std::vector<std::function<void()>>&& tasks) {
	using namespace std::chrono_literals;
	if (tasks.empty())
		return;

	size_t thread_count = std::min(num_threads(), tasks.size());
	std::vector<std::thread> threads;
	threads.reserve(thread_count);

	for (size_t i = 0; i < thread_count; ++i) {
		threads.emplace_back(std::move(tasks.back()));
		tasks.pop_back();
	}

	while (threads.size() > 0) {
		std::this_thread::sleep_for(0.5s);

		for (size_t i = 0; i < threads.size(); ++i) {
			if (!threads[i].joinable())
				continue;

			threads[i].join();

			if (tasks.size() > 0) {
				threads[i] = std::thread{ std::move(tasks.back()) };
				tasks.pop_back();
			} else if (i == threads.size() - 1) { // edge case: pop last
				threads.pop_back();
			} else {
				threads[i] = std::move(threads.back());
				threads.pop_back();
			}
		}
	}
}

} // namespace fea
