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

#include "ans_nibble_statistics.h"
#include <array>

// For theoretical background, please refer to Jaroslaw Duda's seminal paper on rANS:
// https://arxiv.org/pdf/1311.2540.pdf

namespace iguana::ans {

    class nibble_statistics::builder final {
        friend nibble_statistics;

    private:
        std::array<std::uint64_t, 16>  m_freqs;
        std::array<std::uint64_t, 17>  m_cum_freqs;

    public:
        builder() noexcept = default;
        ~builder() = default;

        builder(const builder&) = delete;
        builder& operator =(const builder&) = delete;

        builder(builder&&) = default;
        builder& operator =(builder&&) = default;

    public:
        void build(const std::uint8_t *p, std::size_t n) noexcept;

    private:
        void normalize_freqs() noexcept;
        void calc_cum_freqs() noexcept;
        int compute_histogram(const std::uint8_t *p, std::size_t n) noexcept;
    };
}

//

void iguana::ans::nibble_statistics::compute(const std::uint8_t *p, std::size_t n) noexcept {
    builder bld;
    bld.build(p, n);

	for(std::size_t i = 0; i != 16; ++i) {
		m_table[i] = (bld.m_cum_freqs[i] << cumulative_frequency_bits) | bld.m_freqs[i];
	}
}

void iguana::ans::nibble_statistics::builder::build(const std::uint8_t *p, std::size_t n) noexcept {
    memory::zero(m_freqs);
    memory::zero(m_cum_freqs);

	if (n == 0) {
		// Edge case #1: empty input. Arbitrarily assign probability 1/2 to the last two symbols

		m_freqs[14]     = word_M / 2;
		m_freqs[15]     = word_M / 2;
		m_cum_freqs[15] = word_M / 2;
		m_cum_freqs[16] = word_M;
        return;        
    }

	if (const auto non_zero_freq_idx = compute_histogram(p, n); m_freqs[non_zero_freq_idx] == (n * 2)) { // 2* due to working with nibbles
		// Edge case #2: repetition of a single character.
		//
		// The ANS normalized cumulative frequencies by definition must sum up to a power of 2 (=ansNibbleWordM)
		// to allow for fast division/modulo operations implemented with shift/and. The problem is that
		// the sum is exactly a power of 2: it requires N+1 bits to encode the leading one and the N zeros
		// that follow. This can be fixed by introducing an artificial symbol to the alphabet (a nibble with
		// the value of 16) and assiging it the lowest possible normalized probability of occurrence (=1/ansNibbleWordM).
		// It will then be re-scaled to the value of 1, making the preceeding cumulative frequencies sum up
		// precisely to ansNibbleWordM-1, which can be encoded using N bits. Since the symbol is not encodable
		// in 8 bits, it cannot occur in the input data block, and so will never be mistakenly decoded as well.
		// This tricks saves 2 bits that can be reused for other purposes with a negligible impact on the
		// compression ratio. On top of that, it solves the repeated single character degenerate input case,
		// as no symbol can have the probability of ocurrence equal to 1 -- it will be (ansNibbleWordM-1)/ansNibbleWordM
		// in the worst case.

		m_freqs[non_zero_freq_idx] = word_M - 1;
		for(auto i = non_zero_freq_idx + 1; i != 17; ++i) {
			m_cum_freqs[i] = word_M - 1;
		}
		return;
	}

	normalize_freqs();
}

void iguana::ans::nibble_statistics::builder::normalize_freqs() noexcept {
    calc_cum_freqs();
	const std::uint64_t target_total = word_M;
	const std::uint64_t cur_total = m_cum_freqs[16]; // TODO: prefix sum

	// resample distribution based on cumulative freqs
	for(std::size_t i = 1; i != 17; ++i) {
		m_cum_freqs[i] = (target_total * m_cum_freqs[i]) / cur_total;
	}

	// if we nuked any non-0 frequency symbol to 0, we need to steal
	// the range to make the frequency nonzero from elsewhere.
	//
	// this is not at all optimal, i'm just doing the first thing that comes to mind.
	for(int i = 0; i != 16; ++i) {
		if ((m_freqs[i] != 0) && (m_cum_freqs[i+1] == m_cum_freqs[i])) {
			// symbol i was set to zero freq
			// find best symbol to steal frequency from (try to steal from low-freq ones)
			auto best_freq  = ~std::uint64_t(0);
			auto best_steal = -1;
			for(int j = 0; j != 16; ++j) {
				const auto freq = m_cum_freqs[j+1] - m_cum_freqs[j];
				if ((freq > 1) && (freq < best_freq)) {
					best_freq = freq;
					best_steal = j;
				}
			}

			// and steal from it!
			if (best_steal < i) {
				for(int j = best_steal + 1; j <= i; ++j) {
					--m_cum_freqs[j];
				}
			} else {
				for(int j = i + 1; j <= best_steal; ++j) {
					++m_cum_freqs[j];
				}
			}
		}
	}

	// calculate updated freqs and make sure we didn't screw anything up
	for(std::size_t i = 0; i != 16; ++i) {
		// calc updated freq
		m_freqs[i] = m_cum_freqs[i+1] - m_cum_freqs[i];
	}
}

void iguana::ans::nibble_statistics::builder::calc_cum_freqs() noexcept {
	// TODO: another prefix sum
	for(std::size_t i = 0; i != 16; ++i) {
		m_cum_freqs[i+1] = m_cum_freqs[i] + m_freqs[i];
	}    
}

int iguana::ans::nibble_statistics::builder::compute_histogram(const std::uint8_t *p, std::size_t n) noexcept {
	// 4-way histogram calculation to compensate for the store-to-load forwarding issues observed here:
	// https://fastcompression.blogspot.com/2014/09/counting-bytes-fast-little-trick-from.html

    if (n == 0) {
        return -1;
    }

	std::uint64_t partial[4][16];
    memory::zero(partial);
    auto k = utils::align_down(n, 4);

	for(std::size_t i = 0; i < k; i += 4) {
		++partial[0][p[i+0] & 0x0f];
		++partial[1][p[i+0] >> 4];
		++partial[2][p[i+1] & 0x0f];
		++partial[3][p[i+1] >> 4];
		++partial[0][p[i+2] & 0x0f];
		++partial[1][p[i+2] >> 4];
		++partial[2][p[i+3] & 0x0f];
		++partial[3][p[i+3] >> 4];
	}

	// Process the remainder
	for(std::size_t i = k; i != n; ++i) {
		++partial[0][p[i] & 0x0f];
		++partial[0][p[i] >> 4];
	}

	// Add up all the ways
	for(std::size_t i = 0; i != 16; ++i) {
		m_freqs[i] = partial[0][i] + partial[1][i] + partial[2][i] + partial[3][i];
	}

	// Find the index of some non-zero freq
	for(std::size_t i = 0; i != 16; ++i) {
		if (m_freqs[i] != 0) {
			return i;
		}
	}

    return -1; // Unreachable, as the n == 0 case was handled at the beginning
}
