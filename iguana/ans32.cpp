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

iguana::ans32::encoder::encoder() {
    memory::fill(m_state, ans::word_L);
    m_rev.reserve(ans::initial_buffer_size);
}

iguana::ans32::encoder::~encoder() noexcept {}
  
// This experimental arithmetic compression/decompression functionality is based on
// the work of Fabian Giesen, available here: https://github.com/rygorous/ryg_rans
// and kindly placed in the Public Domain per the CC0 licence:
// https://github.com/rygorous/ryg_rans/blob/master/LICENSE
//
// For theoretical background, please refer to Jaroslaw Duda's seminal paper on rANS:
// https://arxiv.org/pdf/1311.2540.pdf

void iguana::ans32::encoder::put(output_stream& dst, const ans::statistics& stats, const std::uint8_t* p, std::size_t n) {
	// the forward half
	for(int lane = 15; lane >= 0; --lane) {
		if (lane < n) {
			const auto q = stats[p[lane]];
			const auto freq = q & ans::statistics::frequency_mask;
			const auto start = (q >> ans::statistics::frequency_bits) & ans::statistics::cumulative_frequency_mask;
			// renormalize
			auto x = m_state[lane];
			if (x >= ((ans::word_L >> ans::word_M_bits) << ans::word_L_bits) * freq) {
				dst.append_big_endian(static_cast<std::uint16_t>(x));
				x >>= ans::word_L_bits;
			}
			// x = C(s,x)
			m_state[lane] = ((x / freq) << ans::word_M_bits) + (x % freq) + start;
		}
	}
	// the reverse half
	for(int lane = 31; lane >= 16; --lane) {
		if (lane < n) {
			const auto q = stats[p[lane]];
			const auto freq = q & ans::statistics::frequency_mask;
			const auto start = (q >> ans::statistics::frequency_bits) & ans::statistics::cumulative_frequency_mask;
			// renormalize
			auto x = m_state[lane];
			if (x >= ((ans::word_L >> ans::word_M_bits) << ans::word_L_bits) * freq) {
				m_rev.append_little_endian(static_cast<std::uint16_t>(x));
				x >>= ans::word_L_bits;
			}
			// x = C(s,x)
			m_state[lane] = ((x / freq) << ans::word_M_bits) + (x % freq) + start;
		}
	}
}

void iguana::ans32::encoder::flush(output_stream& dst) {
	for(int lane = 15; lane >= 0; --lane) {
        dst.append_big_endian(m_state[lane]);
	}
	for(int lane = 16; lane < 32; ++lane) {
        m_rev.append_little_endian(m_state[lane]);
	}
}

void iguana::ans32::encoder::encode(output_stream& dst, const ans::statistics& stats, const std::uint8_t *src, std::size_t src_len) {
    memory::fill(m_state, ans::word_L);
    m_rev.clear();

    if (const auto ec = compress(dst, stats, src, src_len); ec != error_code::ok) {
        exception::from_error(ec);
    }

	const auto len_rev = m_rev.size();
    dst.reserve_more(len_rev + ans::dense_table_max_length);
	dst.append_reverse(m_rev.data(), m_rev.size());
}

iguana::error_code iguana::ans32::encoder::compress_portable(output_stream& dst, const ans::statistics& stats, const std::uint8_t *src, std::size_t src_len) {
	const auto n_last = src_len % 32;
	long k = src_len - n_last;

	// Process the last chunk first
	put(dst, stats, src + k, n_last);

	// Process the remaining chunks
	for(k -= 32; k >= 0; k -= 32) {
		put(dst, stats, src + k, 32);
	}
	flush(dst);
    return error_code::ok;
}
