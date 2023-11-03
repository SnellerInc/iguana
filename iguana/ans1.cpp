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

#include "ans1.h"

namespace iguana::ans1 {
    void (*encoder::g_Compress)(context& ctx) = &encoder::compress_portable;
    const internal::initializer<encoder> encoder::g_Initializer;

    void (*decoder::g_Decompress)(context& ctx) = &decoder::decompress_portable;
    const internal::initializer<decoder> decoder::g_Initializer;
}

iguana::ans1::encoder::~encoder() noexcept {}

// This experimental arithmetic compression/decompression functionality is based on
// the work of Fabian Giesen, available here: https://github.com/rygorous/ryg_rans
// and kindly placed in the Public Domain per the CC0 licence:
// https://github.com/rygorous/ryg_rans/blob/master/LICENSE
//
// For theoretical background, please refer to Jaroslaw Duda's seminal paper on rANS:
// https://arxiv.org/pdf/1311.2540.pdf

void iguana::ans1::encoder::encode(output_stream& dst, const statistics& stats, const std::uint8_t *src, std::size_t src_len) {
    context ctx { .dst = dst, .stats = stats, .src = src, .src_len = src_len };
    g_Compress(ctx);

    if (ctx.ec != error_code::ok) {
        exception::from_error(ctx.ec);
    }
    dst.reserve_more(statistics::dense_table_max_length);
}

void iguana::ans1::encoder::compress_portable(context& ctx) {
    std::uint32_t state = statistics::word_L;
    
	for(auto *p = ctx.src + ctx.src_len; p > ctx.src;) {
        const std::uint8_t v = *--p;
        const auto q = ctx.stats[v];
        const auto freq = q & statistics::frequency_mask;
        const auto start = (q >> statistics::frequency_bits) & statistics::cumulative_frequency_mask;
        // renormalize
        auto x = state;
        if (x >= ((statistics::word_L >> statistics::word_M_bits) << statistics::word_L_bits) * freq) {
            ctx.dst.append_little_endian(static_cast<std::uint16_t>(x));
            x >>= statistics::word_L_bits;
        }
        // x = C(s,x)
        state = ((x / freq) << statistics::word_M_bits) + (x % freq) + start;
	}

    ctx.dst.append_little_endian(state);
	ctx.ec = error_code::ok;    
}

void iguana::ans1::encoder::at_process_start() {}

void iguana::ans1::encoder::at_process_end() {}

iguana::ans1::decoder::~decoder() noexcept {}

void iguana::ans1::decoder::decode(output_stream& dst, std::size_t result_size, input_stream& src, const statistics::decoding_table& tab) {
    dst.reserve_more(result_size);
    context ctx{ .dst = dst, .result_size = result_size, .src = src, .tab = tab };
    g_Decompress(ctx);

    if (ctx.ec != error_code::ok) {
        exception::from_error(ctx.ec);
    }
}        

void iguana::ans1::decoder::decompress_portable(context& ctx) {
	const auto src_len = ctx.src.size();

	if (src_len < 4) {
		ctx.ec = error_code::wrong_source_size;
        return;
	}

	auto cursor_src = src_len - 4;
    const std::uint8_t* const src = ctx.src.data();
	auto state = utils::read_little_endian<std::uint32_t>(src + cursor_src);
    std::size_t cursor_dst = 0;

	for(;;) {
		{   const std::uint32_t x = state;
            const auto slot = x & (statistics::word_M - 1);
            const auto t = ctx.tab[slot];
            const auto freq = t & (statistics::word_M - 1);
            const auto bias = (t >> statistics::word_M_bits) & (statistics::word_M - 1);
            // s, x = D(x)
            state = freq * (x >> statistics::word_M_bits) + bias;
            ctx.dst.append(static_cast<std::uint8_t>(t >> 24));
        
            if (++cursor_dst >= ctx.result_size) {
                break;
            }
        }

		// Normalize state
		if (const auto x = state; x < statistics::word_L) {
			const auto v = utils::read_little_endian<std::uint16_t>(src + cursor_src - 2);
			cursor_src -= 2;
			state = (x << statistics::word_L_bits) | std::uint32_t(v);
		}
	}

    if (state != statistics::word_L) {
        ctx.ec = error_code::corrupted_bitstream;
        return;
    }

	ctx.ec = error_code::ok;
}

void iguana::ans1::decoder::at_process_start() {}

void iguana::ans1::decoder::at_process_end() {}
