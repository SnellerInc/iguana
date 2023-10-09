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

namespace iguana::ans {
    class statistics::builder {
        friend statistics;

    private:
        std::array<std::uint32_t, 256>  m_freqs;
        std::array<std::uint32_t, 257>  m_cum_freqs;   

    public:
        builder() noexcept = default;
        ~builder() = default;

    public:
        void build(const std::uint8_t *p, std::size_t n) noexcept;

    private:
        void normalize_freqs() noexcept;
        void calc_cum_freqs() noexcept;
        int compute_histogram(const std::uint8_t *p, std::size_t n) noexcept;
    };
}

void iguana::ans::statistics::builder::build(const std::uint8_t *p, std::size_t n) noexcept {
    memory::zero(m_freqs); 
    memory::zero(m_cum_freqs); 

	if (n == 0) {
		// Edge case #1: empty input. Arbitrarily assign probability 1/2 to the last two symbols

        m_freqs[254]     = word_M / 2;
        m_freqs[255]     = word_M / 2;
        m_cum_freqs[255] = word_M / 2;
        m_cum_freqs[256] = word_M;
		return;
	}

    const auto non_zero_freq_idx = compute_histogram(p, n);

	if (m_freqs[non_zero_freq_idx] == n) {
		// Edge case #2: repetition of a single character.
		//
		// The ANS normalized cumulative frequencies by definition must sum up to a power of 2 (=ansWordM)
		// to allow for fast division/modulo operations implemented with shift/and. The problem is that
		// the sum is exactly a power of 2: it requires N+1 bits to encode the leading one and the N zeros
		// that follow. This can be fixed by introducing an artificial symbol to the alphabet (a byte with
		// the value of 256) and assiging it the lowest possible normalized probability of occurrence (=1/ansWordM).
		// It will then be re-scaled to the value of 1, making the preceeding cumulative frequencies sum up
		// precisely to ansWordM-1, which can be encoded using N bits. Since the symbol is not encodable
		// in 8 bits, it cannot occur in the input data block, and so will never be mistakenly decoded as well.
		// This tricks saves 2 bits that can be reused for other purposes with a negligible impact on the
		// compression ratio. On top of that, it solves the repeated single character degenerate input case,
		// as no symbol can have the probability of ocurrence equal to 1 -- it will be (ansWordM-1)/ansWordM
		// in the worst case.

		m_freqs[non_zero_freq_idx] = word_M - 1;
		for(auto i = non_zero_freq_idx + 1; i != m_freqs.size(); ++i) {
			m_cum_freqs[i] = word_M - 1;
		}
		return;
	}

	normalize_freqs();
}

void iguana::ans::statistics::builder::normalize_freqs() noexcept {
    calc_cum_freqs();        
	const std::size_t target_total = word_M;
	const std::size_t cur_total = m_cum_freqs[256]; // TODO: prefix sum

	// resample distribution based on cumulative freqs
	for(std::size_t i = 1; i <= 256; ++i) {
		m_cum_freqs[i] = std::size_t((uint64_t(target_total) * uint64_t(m_cum_freqs[i])) / uint64_t(cur_total));
	}

	// if we nuked any non-0 frequency symbol to 0, we need to steal
	// the range to make the frequency nonzero from elsewhere.
	//
	// this is not at all optimal, i'm just doing the first thing that comes to mind.
	for(std::size_t i = 0; i != 256; ++i) {
		if ((m_freqs[i] != 0) && (m_cum_freqs[i+1] == m_cum_freqs[i])) {
			// symbol i was set to zero freq

			// find best symbol to steal frequency from (try to steal from low-freq ones)
			auto bestFreq = ~std::size_t(0);
			auto bestSteal = -1;
			for(std::size_t j = 0; j != 256; ++j) {
				const auto freq = m_cum_freqs[j+1] - m_cum_freqs[j];
				if ((freq > 1) && (freq < bestFreq)) {
					bestFreq = freq;
					bestSteal = j;
				}
			}

			// and steal from it!
			if (bestSteal < i) {
				for(auto j = bestSteal + 1; j <= i; ++j) {
					--m_cum_freqs[j];
				}
			} else {
				for(auto j = i + 1; j <= bestSteal; ++j) {
					++m_cum_freqs[j];
				}
			}
		}
	}

	// calculate updated freqs and make sure we didn't screw anything up
	for(std::size_t i = 0; i != 256; ++i) {
		// calc updated freq
		m_freqs[i] = m_cum_freqs[i+1] - m_cum_freqs[i];
	}
}

void iguana::ans::statistics::builder::calc_cum_freqs() noexcept {
	// TODO: another prefix sum
	for(auto i = 0; i != m_freqs.size(); ++i) {
		m_cum_freqs[i+1] = m_cum_freqs[i] + m_freqs[i];
	}
}

int iguana::ans::statistics::builder::compute_histogram(const std::uint8_t *p, std::size_t n) noexcept {
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
		m_freqs[i] = partial[0][i] + partial[1][i] + partial[2][i] + partial[3][i];
	}

	// Find the index of some non-zero freq
	for(int i = 0; i != 256; ++i) {
		if (m_freqs[i] != 0) {
			return i;
		}
	}

    return -1; // Unreachable, as the n == 0 case was handled at the beginning    
}

void iguana::ans::statistics::compute(const std::uint8_t *p, std::size_t n) noexcept {
    builder bld;
    bld.build(p, n);
    
	for(std::size_t i = 0; i != 256; ++i) {
		m_table[i] = (bld.m_cum_freqs[i] << cumulative_frequency_bits) | bld.m_freqs[i];
	}
}
