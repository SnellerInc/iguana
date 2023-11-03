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
#include "memops.h"
#include "utils.h"

//

namespace iguana::ans32 {
    void (*encoder::g_Compress)(context& ctx) = &encoder::compress_portable;
    const internal::initializer<encoder> encoder::g_Initializer;

    void (*decoder::g_Decompress)(context& ctx) = &decoder::decompress_portable;
    const internal::initializer<decoder> decoder::g_Initializer;
}

//

iguana::ans32::encoder::encoder() {
    m_rev.reserve(statistics::initial_buffer_size);
}

iguana::ans32::encoder::~encoder() noexcept {}
  
// This experimental arithmetic compression/decompression functionality is based on
// the work of Fabian Giesen, available here: https://github.com/rygorous/ryg_rans
// and kindly placed in the Public Domain per the CC0 licence:
// https://github.com/rygorous/ryg_rans/blob/master/LICENSE
//
// For theoretical background, please refer to Jaroslaw Duda's seminal paper on rANS:
// https://arxiv.org/pdf/1311.2540.pdf

void iguana::ans32::encoder::put(context& ctx, const std::uint8_t* p, std::size_t n) {
	// the forward half
	for(int lane = 15; lane >= 0; --lane) {
		if (lane < n) {
			const auto q = ctx.stats[p[lane]];
			const auto freq = q & statistics::frequency_mask;
			const auto start = (q >> statistics::frequency_bits) & statistics::cumulative_frequency_mask;
			// renormalize
			auto x = ctx.state[lane];
			if (x >= ((statistics::word_L >> statistics::word_M_bits) << statistics::word_L_bits) * freq) {
				ctx.fwd.append_big_endian(static_cast<std::uint16_t>(x));
				x >>= statistics::word_L_bits;
			}
			// x = C(s,x)
			ctx.state[lane] = ((x / freq) << statistics::word_M_bits) + (x % freq) + start;
		}
	}
	// the reverse half
	for(int lane = 31; lane >= 16; --lane) {
		if (lane < n) {
			const auto q = ctx.stats[p[lane]];
			const auto freq = q & statistics::frequency_mask;
			const auto start = (q >> statistics::frequency_bits) & statistics::cumulative_frequency_mask;
			// renormalize
			auto x = ctx.state[lane];
			if (x >= ((statistics::word_L >> statistics::word_M_bits) << statistics::word_L_bits) * freq) {
				ctx.rev.append_little_endian(static_cast<std::uint16_t>(x));
				x >>= statistics::word_L_bits;
			}
			// x = C(s,x)
			ctx.state[lane] = ((x / freq) << statistics::word_M_bits) + (x % freq) + start;
		}
	}
}

void iguana::ans32::encoder::encode(output_stream& dst, const statistics& stats, const std::uint8_t *src, std::size_t src_len) {
    m_rev.clear();
    context ctx { .fwd = dst, .rev = m_rev, .stats = stats, .src = src, .src_len = src_len };
    memory::fill(ctx.state, statistics::word_L);
    g_Compress(ctx);
        
    if (ctx.ec != error_code::ok) {
        exception::from_error(ctx.ec);
    }

	const auto len_rev = m_rev.size();
    dst.reserve_more(len_rev + statistics::dense_table_max_length);
	dst.append_reverse(m_rev.data(), m_rev.size());
}

void iguana::ans32::encoder::compress_portable(context& ctx) {
	const auto n_last = ctx.src_len % 32;
	long k = ctx.src_len - n_last;

	// Process the last chunk first
	put(ctx, ctx.src + k, n_last);

	// Process the remaining chunks
	for(k -= 32; k >= 0; k -= 32) {
		put(ctx, ctx.src + k, 32);
	}

    // Flush
	for(int lane = 15; lane >= 0; --lane) {
        ctx.fwd.append_big_endian(ctx.state[lane]);
	}

	for(int lane = 16; lane < 32; ++lane) {
        ctx.rev.append_little_endian(ctx.state[lane]);
	}

    ctx.ec = error_code::ok;
}

void iguana::ans32::encoder::at_process_start() {}

void iguana::ans32::encoder::at_process_end() {}

//

iguana::ans32::decoder::~decoder() noexcept {}

void iguana::ans32::decoder::decode(output_stream& dst, std::size_t result_size, input_stream& src, const statistics::decoding_table& tab) {
    dst.reserve_more(result_size);
    context ctx{ .dst = dst, .result_size = result_size, .src = src, .tab = tab };
    g_Decompress(ctx);

    if (ctx.ec != error_code::ok) {
        exception::from_error(ctx.ec);
    }
}        

void iguana::ans32::decoder::decompress_portable(context& ctx) {
	std::uint32_t state[32];
	std::size_t cursor_fwd = 64;
	std::size_t cursor_rev = ctx.src.size() - 64;
    const std::uint8_t* const src = ctx.src.data();

	for(std::size_t lane = 0; lane != 16; ++lane) {
		state[lane]    = utils::read_little_endian<std::uint32_t>(src + lane * 4);
		state[lane+16] = utils::read_little_endian<std::uint32_t>(src + lane * 4 + cursor_rev);
	}

	std::size_t cursor_dst = 0;

	for(;;) {
		for(std::size_t lane = 0; lane != 32; ++lane) {
			std::uint32_t x = state[lane];
			const auto slot = x & (statistics::word_M - 1);
			const auto t = ctx.tab[slot];
			const auto freq = std::uint32_t(t & (statistics::word_M - 1));
			const auto bias = std::uint32_t((t >> statistics::word_M_bits) & (statistics::word_M - 1));
			// s, x = D(x)
			state[lane] = freq * (x >> statistics::word_M_bits) + bias;
			const auto s = std::uint8_t(t >> 24);
			if (cursor_dst < ctx.result_size) {
				ctx.dst.append(s);
				++cursor_dst;
			} else {
				goto done;
			}
		}
		// Normalize the forward part
		for(std::size_t lane = 0; lane != 16; ++lane) {
			if (const auto x = state[lane]; x < statistics::word_L) {
				const auto v = utils::read_little_endian<std::uint16_t>(src + cursor_fwd);
				cursor_fwd += 2;
				state[lane] = (x << statistics::word_L_bits) | std::uint32_t(v);
			}
		}
		// Normalize the reverse part
		for(std::size_t lane = 16; lane != 32; ++lane) {
			if (const auto x = state[lane]; x < statistics::word_L) {
				const auto v = utils::read_little_endian<std::uint16_t>(src + cursor_rev - 2);
				cursor_rev -= 2;
				state[lane] = (x << statistics::word_L_bits) | std::uint32_t(v);
			}
		}
	}

done:

    for(std::size_t i = 0; i != 32; ++i) {
        if (state[i] != statistics::word_L) {
            ctx.ec = error_code::corrupted_bitstream;
            return;
        }
    }

    ctx.ec = error_code::ok;
}

void iguana::ans32::decoder::at_process_start() {}

void iguana::ans32::decoder::at_process_end() {}
