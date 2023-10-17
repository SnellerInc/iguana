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

#include <vector>
#include "ans_byte_statistics.h"
#include "utils.h"

//

namespace iguana::ans {

    class byte_statistics::builder final {
        friend byte_statistics;

    private:
        std::array<std::uint32_t, 256>  m_freqs;
        std::array<std::uint32_t, 257>  m_cum_freqs;

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

    //

    class byte_statistics::bitstream final {
  	    std::uint64_t               m_acc = 0;
	    int                         m_cnt = 0;
        std::vector<std::uint8_t>   m_buf;

    public:
        bitstream() = default;
        ~bitstream() noexcept = default;

        bitstream(const bitstream&) = delete;
        bitstream& operator =(const bitstream&) = delete;

        bitstream(bitstream&&) = default;
        bitstream& operator =(bitstream&&) = default;

    public:
        void append(std::uint32_t v, std::uint32_t k);
        void flush();

        auto size() const noexcept {
            return m_buf.size();
        }

        auto data() const noexcept {
            return m_buf.data();
        }
    };
}

void iguana::ans::byte_statistics::builder::build(const std::uint8_t *p, std::size_t n) noexcept {
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

void iguana::ans::byte_statistics::builder::normalize_freqs() noexcept {
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

void iguana::ans::byte_statistics::builder::calc_cum_freqs() noexcept {
	// TODO: another prefix sum
	for(auto i = 0; i != m_freqs.size(); ++i) {
		m_cum_freqs[i+1] = m_cum_freqs[i] + m_freqs[i];
	}
}

int iguana::ans::byte_statistics::builder::compute_histogram(const std::uint8_t *p, std::size_t n) noexcept {
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

void iguana::ans::byte_statistics::compute(const std::uint8_t *p, std::size_t n) noexcept {
    builder bld;
    bld.build(p, n);

	for(std::size_t i = 0; i != 256; ++i) {
		m_table[i] = (bld.m_cum_freqs[i] << cumulative_frequency_bits) | bld.m_freqs[i];
	}
}

void iguana::ans::byte_statistics::serialize(output_stream& s) const {
    bitstream ctrl;
    bitstream data;

	for(std::size_t i = 0; i != 256; ++i) {
		const auto f = m_table[i] & frequency_mask;

		// 000 => 0
		// 001 => 1
		// 010 => 2
		// 011 => 3
		// 100 => 4
		// 101 => one nibble f - 5
		// 110 => two nibbles f - 21
		// 111 => three nibbles f - 277

		if (f < 5) {
			ctrl.append(f, 3);
		} else if (f < 21) {
			ctrl.append(0b101, 3);
			data.append(f-5, 4);
		} else if (f < 277) {
			ctrl.append(0b110, 3);
			data.append(f-21, 8);
		} else {
			ctrl.append(0b111, 3);
			data.append(f-277, 12);
		}
	}

	ctrl.flush();
	data.flush();

	const auto n_ctrl = ctrl.size();
	const auto n_data = data.size();
    s.reserve_more(n_ctrl + n_data);
    s.append_reverse(data.data(), data.size());
    s.append(ctrl.data(), ctrl.size());
}

void iguana::ans::byte_statistics::deserialize(input_stream& s) {
	// The code part is encoded as 256 3-bit values, making it 96 bytes in total.
	// Decoding it in groups of 24 bits is convenient: 8 words at a time.

	const auto src_len = s.size();
	if (src_len < ctrl_block_size) {
        throw wrong_source_size_exception();
	}

    const std::uint8_t* const ctrl = s.data() + src_len - ctrl_block_size;
	ssize_t nibidx = (src_len - ctrl_block_size - 1) * 2 + 1;
	std::uint32_t k = 0;

	for(std::size_t i = 0; i != ctrl_block_size; i += 3) {
		std::uint32_t x = std::uint32_t(ctrl[i]) | std::uint32_t(ctrl[i+1])<<8 | std::uint32_t(ctrl[i+2]) << 16;
		// Eight 3-bit control words fit within a single 24-bit chunk
		for(std::size_t j = 0; j != 8; ++j, ++k) {
			const auto v = x & 0x07;
			x >>= 3;
			switch(v) {
			case 0b111: {
				// Three nibbles f - 277
                const auto x0 = fetch_nibble(s, nibidx);
                const auto x1 = fetch_nibble(s, nibidx);
                const auto x2 = fetch_nibble(s, nibidx);
				m_table[k] = (x0 | (x1 << 4) | (x2 << 8)) + 277;
            } break;

			case 0b110: {
				// Two nibbles f - 21
                const auto x0 = fetch_nibble(s, nibidx);
                const auto x1 = fetch_nibble(s, nibidx);
				m_table[k] = (x0 | (x1 << 4)) + 21;
            } break;

			case 0b101: {
                // One nibble f - 5
                const auto x0 = fetch_nibble(s, nibidx);
                m_table[k] = x0 + 5;
            } break;

			default:
				// Explicit encoding of a short value
				m_table[k] = v;
                break;
			}
		}
	}
    s.set_end(s.data() + ((nibidx + 1) >> 1));
}

void iguana::ans::byte_statistics::build_decoding_table(decoding_table& tab) const noexcept {
	// The normalized frequencies have been recovered. Fill the decoding table accordingly.
    std::size_t start = 0;
    for(std::uint32_t sym = 0; sym != 256; ++sym) {
        const auto freq = m_table[sym];
        for(std::uint32_t i = 0; i < freq; ++i) {
            const auto slot = start + i;
            tab[slot] = (sym << 24) | (i << word_M_bits) | freq;
        }
        start += freq;
    }
}

std::uint32_t iguana::ans::byte_statistics::fetch_nibble(input_stream& s, ssize_t& idx) {
	if (idx < 0) {
		throw out_of_input_data_exception();
	}

    const std::uint8_t x = s[idx >> 1];
    const auto prev_idx = idx--;

	if ((prev_idx & 0x01) != 0) {
		return std::uint32_t(x & 0x0f);
	} else {
		return std::uint32_t(x >> 4);
	}
}

void iguana::ans::byte_statistics::bitstream::append(std::uint32_t v, std::uint32_t k) {
	const std::uint32_t m = ~(~std::uint32_t(0) << k);
	m_acc |= std::uint64_t(v & m) << m_cnt;
	m_cnt += int(k);

	while(m_cnt >= 8) {
		m_buf.push_back(std::uint8_t(m_acc));
		m_acc >>= 8;
		m_cnt -= 8;
	}
}

void iguana::ans::byte_statistics::bitstream::flush() {
	while(m_cnt > 0) {
		m_buf.push_back(std::uint8_t(m_acc));
		m_acc >>= 8;
		m_cnt -= 8;
	}
}
