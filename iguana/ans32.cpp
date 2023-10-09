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

#include "ans32.h"

//

iguana::ans32::encoder::encoder() noexcept {
    memory::fill(m_state, ans::word_L);
    m_buf_fwd.reserve(ans::initial_buffer_size);
    m_buf_rev.reserve(ans::initial_buffer_size);
}
  
// This experimental arithmetic compression/decompression functionality is based on
// the work of Fabian Giesen, available here: https://github.com/rygorous/ryg_rans
// and kindly placed in the Public Domain per the CC0 licence:
// https://github.com/rygorous/ryg_rans/blob/master/LICENSE
//
// For theoretical background, please refer to Jaroslaw Duda's seminal paper on rANS:
// https://arxiv.org/pdf/1311.2540.pdf

void iguana::ans32::encoder::put(const std::uint8_t* p, std::size_t avail, const ans::statistics& stats) {
	// the forward half
	for(int lane = 15; lane >= 0; --lane) {
		if (lane < avail) {
			const auto q = stats[p[lane]];
			const auto freq = q & ans::statistics::frequency_mask;
			const auto start = (q >> ans::statistics::frequency_bits) & ans::statistics::cumulative_frequency_mask;
			// renormalize
			auto x = m_state[lane];
			if (x >= ((ans::word_L >> ans::word_M_bits) << ans::word_L_bits) * freq) {
				m_buf_fwd.append_big_endian(static_cast<std::uint16_t>(x));
				x >>= ans::word_L_bits;
			}
			// x = C(s,x)
			m_state[lane] = ((x / freq) << ans::word_M_bits) + (x % freq) + start;
		}
	}
	// the reverse half
	for(int lane = 31; lane >= 16; --lane) {
		if (lane < avail) {
			const auto q = stats[p[lane]];
			const auto freq = q & ans::statistics::frequency_mask;
			const auto start = (q >> ans::statistics::frequency_bits) & ans::statistics::cumulative_frequency_mask;
			// renormalize
			auto x = m_state[lane];
			if (x >= ((ans::word_L >> ans::word_M_bits) << ans::word_L_bits) * freq) {
				m_buf_rev.append_little_endian(static_cast<std::uint16_t>(x));
				x >>= ans::word_L_bits;
			}
			// x = C(s,x)
			m_state[lane] = ((x / freq) << ans::word_M_bits) + (x % freq) + start;
		}
	}
}

void iguana::ans32::encoder::flush() {
	for(int lane = 15; lane >= 0; --lane) {
        m_buf_fwd.append_big_endian(m_state[lane]);
	}
	for(int lane = 16; lane < 32; ++lane) {
        m_buf_rev.append_little_endian(m_state[lane]);
	}
}

iguana::error_code iguana::ans32::encoder::encode(const std::uint8_t *src, std::size_t n) {
    return encode(src, n, ans::statistics(src, n));
}

iguana::error_code iguana::ans32::encoder::encode(const std::uint8_t *src, std::size_t n, const ans::statistics& stats) {
    clear();
    compress(src, n, stats);
    const auto len_fwd = m_buf_fwd.size();
	const auto len_rev = m_buf_rev.size();
    m_buf_fwd.reserve(len_fwd + len_rev + ans::dense_table_max_length);
/*TODO:
	// In-place inversion of bufFwd
	for i, j := 0, lenFwd-1; i < j; i, j = i+1, j-1 {
		buf[i], buf[j] = buf[j], buf[i]
	}
*/
	m_buf_fwd.append(m_buf_rev);
	return error_code::ok;
}

void iguana::ans32::encoder::clear() {
    memory::fill(m_state, ans::word_L);
    m_buf_fwd.clear();
    m_buf_rev.clear();
}

void iguana::ans32::encoder::compress_portable(const std::uint8_t *src, std::size_t n, const ans::statistics& stats) {
	const auto n_last = n % 32;
	long k = n - n_last;

	// Process the last chunk first
	put(src + k, n_last, stats);

	// Process the remaining chunks
	for(k -= 32; k >= 0; k -= 32) {
		put(src + k, 32, stats);
	}
	flush();
}
