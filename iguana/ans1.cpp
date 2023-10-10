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

iguana::ans1::encoder::encoder()
  : m_state(ans::word_L) {
    m_buf.reserve(ans::initial_buffer_size);
}

iguana::ans1::encoder::~encoder() noexcept {}

// This experimental arithmetic compression/decompression functionality is based on
// the work of Fabian Giesen, available here: https://github.com/rygorous/ryg_rans
// and kindly placed in the Public Domain per the CC0 licence:
// https://github.com/rygorous/ryg_rans/blob/master/LICENSE
//
// For theoretical background, please refer to Jaroslaw Duda's seminal paper on rANS:
// https://arxiv.org/pdf/1311.2540.pdf

void iguana::ans1::encoder::put(std::uint8_t v, const ans::statistics& stats) {
	const auto q = stats[v];
    const auto freq = q & ans::statistics::frequency_mask;
    const auto start = (q >> ans::statistics::frequency_bits) & ans::statistics::cumulative_frequency_mask;
	// renormalize
	auto x = m_state;
	if (x >= ((ans::word_L >> ans::word_M_bits) << ans::word_L_bits) * freq) {
		m_buf.append_little_endian(static_cast<std::uint16_t>(x));
		x >>= ans::word_L_bits;
	}
	// x = C(s,x)
	m_state = ((x / freq) << ans::word_M_bits) + (x % freq) + start;
}

void iguana::ans1::encoder::flush() {
    m_buf.append_little_endian(m_state);
}

iguana::error_code iguana::ans1::encoder::encode(const std::uint8_t *src, std::size_t n, const ans::statistics& stats) {
    clear();
    compress(src, n, stats);
	const auto len_buf = m_buf.size();
    m_buf.reserve(len_buf + ans::dense_table_max_length);
	return error_code::ok;
}

void iguana::ans1::encoder::clear() {
    m_state = ans::word_L;
    super::clear();
}

void iguana::ans1::encoder::compress_portable(const std::uint8_t *src, std::size_t n, const ans::statistics& stats) {
	for(auto *p = src + n; p > src;) {
		put(*--p, stats);
	}
	flush();
}






/*

func ANS1Decode(src []byte, dstLen int) ([]byte, error) {
	r, ec := ans1Decode(src, dstLen)
	if ec != ecOK {
		return nil, errs[ec]
	}
	return r, nil
}

func ANS1DecodeExplicit(src []byte, tab *ANSDenseTable, dstLen int, dst []byte) ([]byte, error) {
	r, ec := ans1DecodeExplicit(src, tab, dstLen, dst)
	if ec != ecOK {
		return nil, errs[ec]
	}
	return r, nil
}

func ans1Decode(src []byte, dstLen int) ([]byte, errorCode) {
	dst := make([]byte, 0, dstLen)
	var tab ANSDenseTable
	data, ec := ansDecodeTable(&tab, src)
	if ec != ecOK {
		return nil, ec
	}
	return ans1DecodeExplicit(data, &tab, dstLen, dst)
}

func ans1DecodeExplicit(src []byte, tab *ANSDenseTable, dstLen int, dst []byte) ([]byte, errorCode) {
	r, ec := ans1Decompress(dst, dstLen, src, tab)
	if ec != ecOK {
		return nil, ec
	}
	return r, ecOK
}

func ans1DecompressReference(dst []byte, dstLen int, src []byte, tab *ANSDenseTable) ([]byte, errorCode) {
	lenSrc := len(src)
	if lenSrc < 4 {
		return nil, ecWrongSourceSize
	}
	cursorSrc := lenSrc - 4
	state := binary.LittleEndian.Uint32(src[cursorSrc:])
	cursorDst := 0

	for {
		x := state
		slot := x & (ansWordM - 1)
		t := tab[slot]
		freq := uint32(t & (ansWordM - 1))
		bias := uint32((t >> ansWordMBits) & (ansWordM - 1))
		// s, x = D(x)
		state = freq*(x>>ansWordMBits) + bias
		s := byte(t >> 24)
		if cursorDst < dstLen {
			dst = append(dst, s)
			cursorDst++
		} else {
			break
		}

		// Normalize state
		if x := state; x < ansWordL {
			v := binary.LittleEndian.Uint16(src[cursorSrc-2:])
			cursorSrc -= 2
			state = (x << ansWordLBits) | uint32(v)
		}
	}
	return dst, ecOK
}

func init() {
	if ansWordMBits > 12 {
		panic("the value of ansWordMBits must not exceed 12")
	}
}

var ans1Compress func(enc *ANS1Encoder) = ans1CompressReference
var ans1Decompress func(dst []byte, dstLen int, src []byte, tab *ANSDenseTable) ([]byte, errorCode) = ans1DecompressReference
*/