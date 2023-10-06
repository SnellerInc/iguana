// Copyright 2023 Sneller, Inc.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "ans_statistics.h"
#include "utils.h"

int iguana::ans::histogram::compute(std::uint8_t *p, std::size_t n) noexcept {
	// 4-way histogram calculation to compensate for the store-to-load forwarding issues observed here:
	// https://fastcompression.blogspot.com/2014/09/counting-bytes-fast-little-trick-from.html

    if (n == 0) {
        return -1;
    }

	std::size_t partial[4][256];
    memory::zero(partial);
    auto k = utils::align_down(n, 4);
    
	for(std::size_t i = 0; i != k; i += 4) {
		++partial[0][p[i+0]];
		++partial[1][p[i+1]];
		++partial[2][p[i+2]];
		++partial[3][p[i+3]];
	}

	// Process the remainder
	for(; k != n; ++k) {
		++partial[0][p[k]];
	}

	// Add up all the ways
	for(std::size_t i = 0; i != 256; ++i) {
		freqs[i] = partial[0][i] + partial[1][i] + partial[2][i] + partial[3][i];
	}

	// Find the index of some non-zero freq
	for(int i = 0; i != 256; ++i) {
		if (freqs[i] != 0) {
			return i;
		}
	}

    return -1; // Unreachable, as the n == 0 case was handled at the beginning
}
