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

#include <memory>
#include <utility>
#include <stdexcept>
#include "decoder.h"
#include "command.h"
#include "entropy.h"
#include "ans1.h"
#include "ans32.h"
#include "ans_nibble.h"

//

namespace iguana {
    void (*decoder::g_Decompress)(context& ctx) = &decoder::decompress_portable;
    const internal::initializer<decoder> decoder::g_Initializer;

    //

    static constexpr const std::uint32_t match_len_bits      = 4;
    static constexpr const std::uint32_t literal_len_bits    = 3;
    static constexpr const std::uint32_t mm_long_offsets     = 16;
    static constexpr const std::int64_t  init_last_offset    = 0;
    static constexpr const std::uint32_t max_short_lit_len   = 7;
    static constexpr const std::uint32_t max_short_match_len = 15;
    static constexpr const std::uint32_t last_long_offset    = 31;

    // We'd like to allow 64-byte loads at the final byte offset
    // for each of the streams, so we need (64 - 1) bytes of valid memory
    // past the end of the buffer.
    static constexpr const std::size_t pad_size = (64 - 1);
}

//

iguana::decoder::~decoder() noexcept {}

void iguana::decoder::decode(output_stream& dst, input_stream& src) {
    ssize_t cursor = src.size();
    if (cursor == 0) {
        throw out_of_input_data_exception();
    }

    --cursor;
    const auto* const p_data = src.data();
	const std::uint64_t uncompressed_len = read_control_var_uint(p_data, cursor);

	if (uncompressed_len == 0) {
		return;
	}

    dst.reserve_more(uncompressed_len);
    decompress(dst, p_data, uncompressed_len, cursor);
}

std::uint64_t iguana::decoder::read_control_var_uint(const std::uint8_t* src, ssize_t& cursor) {
	std::uint64_t r = 0;
	while(cursor >= 0) {
		const std::uint8_t v = src[cursor--];
		r = (r << 7) | std::uint64_t(v & 0x7f);
		if ((v & 0x80) != 0) {
            return r;
		}
	}

    throw out_of_input_data_exception();
}

void iguana::decoder::decompress(output_stream& dst, const std::uint8_t* const src, std::uint64_t uncompressed_len, ssize_t& ctrl_cursor) {
    IGUANA_UNIMPLEMENTED

/*TODO
func (d *Decoder) decode(, dst []byte, src []byte) ([]byte, errorCode) {
	d.reset()
	var ec errorCode
*/

    context ctx{ .dst = dst, .last_offset = 0 };

	// Fetch the header

	for(std::uint64_t data_cursor = 0;;) {
		if (ctrl_cursor < 0) {
            throw out_of_input_data_exception();
		}
		const std::uint8_t cmd = src[ctrl_cursor--];

		switch (static_cast<command>(cmd & command_mask)) {
            case command::copy_raw: {
                const std::uint64_t n = read_control_var_uint(src, ctrl_cursor);
                dst.append(src + data_cursor, n);
                data_cursor += n;
            } break;

            case command::decode_ans32: {
                const std::uint64_t len_uncompressed = read_control_var_uint(src, ctrl_cursor);
                const std::uint64_t len_compressed = read_control_var_uint(src, ctrl_cursor);

                {   typename ans32::decoder::statistics::decoding_table ans_tab;
                    input_stream is{src + data_cursor, std::size_t(len_compressed)};
                    data_cursor += len_compressed;
                    // Recover the ANS decoding table from the input stream                
                    ans32::decoder::statistics{is}.build_decoding_table(ans_tab);

                    // Decode the compressed content
                    ans32::decoder{}.decode(dst, static_cast<std::size_t>(len_uncompressed), is, ans_tab);
                }
            } break;

		case command::decode_ans1: {
                const std::uint64_t len_uncompressed = read_control_var_uint(src, ctrl_cursor);
                const std::uint64_t len_compressed = read_control_var_uint(src, ctrl_cursor);

                 {  ans1::decoder::statistics::decoding_table ans_tab;
                    input_stream is{src + data_cursor, std::size_t(len_compressed)};
                    data_cursor += len_compressed;
                    // Recover the ANS decoding table from the input stream                
                    ans1::decoder::statistics{is}.build_decoding_table(ans_tab);

                    // Decode the compressed content
                    ans1::decoder{}.decode(dst, static_cast<std::size_t>(len_uncompressed), is, ans_tab);
                }
            } break;

		case command::decode_ans_nibble: {
                const std::uint64_t len_uncompressed = read_control_var_uint(src, ctrl_cursor);
                const std::uint64_t len_compressed = read_control_var_uint(src, ctrl_cursor);

                 {  ans_nibble::decoder::statistics::decoding_table ans_tab;
                    input_stream is{src + data_cursor, std::size_t(len_compressed)};
                    data_cursor += len_compressed;
                    // Recover the ANS decoding table from the input stream                
                    ans_nibble::decoder::statistics{is}.build_decoding_table(ans_tab);

                    // Decode the compressed content
                    ans_nibble::decoder{}.decode(dst, static_cast<std::size_t>(len_uncompressed), is, ans_tab);
                }




IGUANA_UNIMPLEMENTED
/*TODO			encoded, ec := ansNibbleDecodeTable(&d.ansnibtab, ans)
			if ec != ecOK {
				return dst, ec
			}
			dst, ec = ansNibbleDecodeExplicit(encoded, &d.ansnibtab, int(lenUncompressed), dst)
			if ec != ecOK {
				return dst, ec
			}
*/
        } break;

		case command::decode_iguana: {
			// Fetch the header byte
			if (ctrl_cursor < 0) {
                throw out_of_input_data_exception();
			}

            const std::uint64_t hdr = read_control_var_uint(src, ctrl_cursor);
        
			// Fetch the uncompressed streams' lengths
			if (hdr == 0) {
				for(std::size_t i = 0; i != substream::count; ++i) {
                    const std::uint64_t u_len = read_control_var_uint(src, ctrl_cursor);
                    ctx.streams[i].set(src + data_cursor, std::size_t(u_len));
                    data_cursor += u_len;
				}
			} else {
				std::uint64_t u_lens[substream::count];
				std::uint64_t entropy_buffer_size = 0;

				for(std::size_t i = 0; i != substream::count; ++i) {
                    const std::uint64_t u_len = read_control_var_uint(src, ctrl_cursor);
					u_lens[i] = u_len;
					if (const auto em = static_cast<entropy_mode>((hdr >> (i * 4)) & 0x0f); em != entropy_mode::none) {
						entropy_buffer_size += u_len;
					}
				}

                m_ent_buf.reset(entropy_buffer_size + pad_size);                

IGUANA_UNIMPLEMENTED
/*TODO
				if uint64(cap(d.entbuf)) < entropyBufferSize+padSize {
					// ensure the output is appropriately padded:
					d.entbuf = make([]byte, entropyBufferSize, entropyBufferSize+padSize)
				}
*/
				std::uint64_t ent_offs = 0;

				for(std::size_t i = 0; i != substream::count; ++i) {
					const auto u_len = u_lens[i];
					if (const auto em = static_cast<entropy_mode>((hdr >> (i * 4)) & 0x0f); em == entropy_mode::none) {
IGUANA_UNIMPLEMENTED
		//TODO				ctx.pack[i].set = d.padStream(i, src[dataCursor:dataCursor+uLen])
						data_cursor += u_len;
					} else {
                        const std::uint64_t c_len = read_control_var_uint(src, ctrl_cursor);
						switch(em) {
						case entropy_mode::ans32: {

                        // Recover the ANS decoding table from the input stream                
                        ans32::decoder::statistics::decoding_table ans_tab;
                        {   input_stream is{src + data_cursor, std::size_t(c_len)};
                            data_cursor += c_len;
                            ans32::decoder::statistics{is}.build_decoding_table(ans_tab);
                        }
                        
                                          
      
IGUANA_UNIMPLEMENTED
/*TODO
							ans := src[dataCursor : dataCursor+cLen]
							dataCursor += cLen

							encoded, ec := ansDecodeTable(&d.anstab, ans)
							if ec != ecOK {
								return dst, ec
							}

							buf := d.entbuf[entOffs:entOffs]
							d.pack[i].data, ec = ans32DecodeExplicit(encoded, &d.anstab, int(uLen), buf)
							if ec != ecOK {
								return dst, ec
							}
							entOffs += uLen*/
                        } break;

						case entropy_mode::ans1: {
IGUANA_UNIMPLEMENTED /*TODO
							ans := src[dataCursor : dataCursor+cLen]
							dataCursor += cLen

							encoded, ec := ansDecodeTable(&d.anstab, ans)
							if ec != ecOK {
								return dst, ec
							}

							buf := d.entbuf[entOffs:entOffs]
							d.pack[i].data, ec = ans1DecodeExplicit(encoded, &d.anstab, int(uLen), buf)
							if ec != ecOK {
								return dst, ec
							}
							entOffs += uLen*/
                        } break;

						case entropy_mode::ans_nibble: {
IGUANA_UNIMPLEMENTED
/*TODO
							ansNib := src[dataCursor : dataCursor+cLen]
							dataCursor += cLen

							encoded, ec := ansNibbleDecodeTable(&d.ansnibtab, ansNib)
							if ec != ecOK {
								return dst, ec
							}

							buf := d.entbuf[entOffs:entOffs]
							d.pack[i].data, ec = ansNibbleDecodeExplicit(encoded, &d.ansnibtab, int(uLen), buf)
							if ec != ecOK {
								return dst, ec
							}
							entOffs += uLen
*/          
                        } break;

						default:
							throw corrupted_bitstream_exception("unrecognized entropy mode");
						}
					}
				}
			}

			ctx.last_offset = init_last_offset;
            g_Decompress(ctx);

            if (ctx.ec != error_code::ok) {
                exception::from_error(ctx.ec);
            }
        } break;

		default:
            throw unrecognized_command_exception();
		}

		if ((cmd & last_command_marker) != 0) {
			return;
		}
	}
}

void iguana::decoder::decompress_portable(context& ctx) {
	// [0_MMMM_LLL] - 16-bit offset, 4-bit match length (4-15+), 3-bit literal length (0-7+)
	// [1_MMMM_LLL] -   last offset, 4-bit match length (0-15+), 3-bit literal length (0-7+)
	// flag 31      - 24-bit offset,        match length (47+),    no literal length
	// flag 0-30    - 24-bit offset,  31 match lengths (16-46),    no literal length

    auto last_offs = ctx.last_offset;

	// Main Loop : decode sequences
	while(!ctx.streams[substream::tokens].empty()) {
		//get literal length
		std::uint32_t match_len = 0;
		const auto token = ctx.streams[substream::tokens].fetch8(ctx.ec);
		if (ctx.ec != error_code::ok) {
			return;
		}

		if (token >= 32) {
			auto lit_len = std::uint32_t(token & max_short_lit_len);
			if (lit_len == max_short_lit_len) {
				const auto val = ctx.streams[substream::var_lit_len].fetch_var_uint(ctx.ec);
		        if (ctx.ec != error_code::ok) {
					return;
				}
				lit_len = val + max_short_lit_len;
			}
			if (lit_len > 0) {
				if (const auto seq = ctx.streams[substream::literals].fetch_sequence(lit_len, ctx.ec); ctx.ec != error_code::ok) {
					return;
				} else {
					ctx.dst.append(seq);
				}
			}

			// get offset
			if ((token & 0x80) == 0) {
				// [0_MMMM_LLL] - 16-bit offset, 4-bit match length (4-15+), 3-bit literal length (0-7+)
				const auto new_offs = ctx.streams[substream::offset16].fetch16(ctx.ec);
		        if (ctx.ec != error_code::ok) {
					return;
				}
				last_offs = -std::int64_t(new_offs);
			}

			// get matchlength
			match_len = (token >> literal_len_bits) & max_short_match_len;
			if (match_len == max_short_match_len) {
				const auto val = ctx.streams[substream::var_match_len].fetch_var_uint(ctx.ec);
		        if (ctx.ec != error_code::ok) {
					return;
				}
				match_len = val + max_short_match_len;
			}
		} else if (token < last_long_offset) {
			// token < 31
			match_len = token + mm_long_offsets;
			const auto x = ctx.streams[substream::offset24].fetch24(ctx.ec);
		    if (ctx.ec != error_code::ok) {
				return;
			}
			last_offs = -std::int64_t(x);
		} else {
			// token == 31
			const auto val = ctx.streams[substream::var_match_len].fetch_var_uint(ctx.ec);
		    if (ctx.ec != error_code::ok) {
				return;
			}
			match_len = val + last_long_offset + mm_long_offsets;
			const auto x = ctx.streams[substream::offset24].fetch24(ctx.ec);
		    if (ctx.ec != error_code::ok) {
				return;
			}
			last_offs = -std::int64_t(x);
		}
		const std::uint32_t match = ctx.dst.size() + last_offs;
		wild_copy(ctx.dst, match, match_len);
	}

	// last literals
	if (const auto remainder_len = ctx.streams[substream::literals].remaining(); remainder_len > 0) {
        const auto seq = ctx.streams[substream::literals].fetch_sequence(remainder_len, ctx.ec);
        if (ctx.ec != error_code::ok) {
			return;
		} else {
			ctx.dst.append(seq);
		}
	}

	// end of decoding
	ctx.last_offset = last_offs;
    ctx.ec = error_code::ok;
}

void iguana::decoder::wild_copy(output_stream& dst, std::size_t offs, std::size_t len) {
IGUANA_UNIMPLEMENTED
/*TODO
// append dst[pos:pos+matchlen] to dst
// taking care to obey overlapped copy semantics
func iguanaWildCopy(dst []byte, pos, matchlen int) []byte {
	if pos+matchlen <= len(dst) {
		// non-overlapped match: just a regular copy
		return append(dst, dst[pos:pos+matchlen]...)
	}
	// slow path: overlapped match;
	// can't copy in units larger than offset distance
	for matchlen > 0 {
		dist := len(dst) - pos
		if matchlen < dist {
			dist = matchlen
		}
		dst = append(dst, dst[pos:pos+dist]...)
		pos += dist
		matchlen -= dist
	}
	return dst
}
*/
}

void iguana::decoder::at_process_start() {
    printf("iguana::decoder::at_process_start()\n");
}

void iguana::decoder::at_process_end() {
    printf("iguana::decoder::at_process_end()\n");
}


std::uint8_t iguana::decoder::substream::fetch8(error_code& ec) noexcept {
    if (empty()) {
        ec = error_code::out_of_input_data;
        return 0;
    }
    ec = error_code::ok;
	return *m_cursor++;
}

std::uint8_t iguana::decoder::substream::fetch8() {
    if (empty()) {
        throw out_of_input_data_exception();
    }
	return *m_cursor++;
}

std::uint16_t iguana::decoder::substream::fetch16(error_code& ec) noexcept {
    if (remaining() < 2) {
        ec = error_code::out_of_input_data;    
        return 0;
    }
    ec = error_code::ok;
    const std::uint8_t a = m_cursor[0];
    const std::uint8_t b = m_cursor[1];
    m_cursor += 2;    
	return std::uint16_t(a) | (std::uint16_t(b) << 8);
}

std::uint16_t iguana::decoder::substream::fetch16() {
    if (remaining() < 2) {
        throw out_of_input_data_exception();
    }
    const std::uint8_t a = m_cursor[0];
    const std::uint8_t b = m_cursor[1];
    m_cursor += 2;    
	return std::uint16_t(a) | (std::uint16_t(b) << 8);
}

std::uint32_t iguana::decoder::substream::fetch24(error_code& ec) noexcept {
    if (remaining() < 3) {
        ec = error_code::out_of_input_data;    
        return 0;
    }
    ec = error_code::ok;
    const std::uint8_t a = m_cursor[0];
    const std::uint8_t b = m_cursor[1];
    const std::uint8_t c = m_cursor[2];
    m_cursor += 3;
	return std::uint32_t(a) | (std::uint32_t(b) << 8) | (std::uint32_t(c) << 16);
}

std::uint32_t iguana::decoder::substream::fetch24() {
    if (remaining() < 3) {
        throw out_of_input_data_exception();
    }
    const std::uint8_t a = m_cursor[0];
    const std::uint8_t b = m_cursor[1];
    const std::uint8_t c = m_cursor[2];
    m_cursor += 3;
	return std::uint32_t(a) | (std::uint32_t(b) << 8) | (std::uint32_t(c) << 16);
}

std::uint32_t iguana::decoder::substream::fetch_var_uint(error_code& ec) noexcept {
    const std::uint8_t a = fetch8(ec);

    if (ec != error_code::ok) {
        return 0;
    }

	if (a < 0xfe) {
		return std::uint32_t(a);
    } else if (a == 0xfe) {
        const std::uint32_t b = fetch16(ec);
        if (ec != error_code::ok) {
            return 0;
        }
		const std::uint32_t x0 = b & 0xff;
		const std::uint32_t x1 = (b >> 8);
		return (x1 * 254) + x0;
    } else {
        const std::uint32_t b = fetch24(ec);
        if (ec != error_code::ok) {
            return 0;
        }
		const std::uint32_t x0 = b & 0xff;
		const std::uint32_t x1 = (b >> 8) & 0xff;
		const std::uint32_t x2 = b >> 16;
		return (((x2 * 254) + x1) * 254) + x0;
    }
}

std::uint32_t iguana::decoder::substream::fetch_var_uint() {
    const std::uint8_t a = fetch8();

	if (a < 0xfe) {
		return std::uint32_t(a);
    } else if (a == 0xfe) {
        const std::uint32_t b = fetch16();
		const std::uint32_t x0 = b & 0xff;
		const std::uint32_t x1 = (b >> 8);
		return (x1 * 254) + x0;
    } else {
        const std::uint32_t b = fetch24();
		const std::uint32_t x0 = b & 0xff;
		const std::uint32_t x1 = (b >> 8) & 0xff;
		const std::uint32_t x2 = b >> 16;
		return (((x2 * 254) + x1) * 254) + x0;
    }
}

iguana::const_byte_span iguana::decoder::substream::fetch_sequence(std::size_t n, error_code& ec) noexcept {
    if (remaining() < n) {
        ec = error_code::out_of_input_data;    
        return {};
    }
    ec = error_code::ok;
    auto * const p = m_cursor;
    m_cursor += n;
    return { p, n };
}

iguana::const_byte_span iguana::decoder::substream::fetch_sequence(std::size_t n) {
    if (remaining() < n) {
        throw out_of_input_data_exception();
    }
    auto * const p = m_cursor;
    m_cursor += n;
    return { p, n };
}


iguana::decoder::entropy_buffer::entropy_buffer(std::size_t n) {
    const auto r = acquire_memory(n);
    m_data = r.first;
    m_cursor = 0;
    m_capacity = r.second;
}

iguana::decoder::entropy_buffer::~entropy_buffer() {
    release_memory(m_data);
}

iguana::decoder::entropy_buffer::entropy_buffer(entropy_buffer&& v) {
    m_data = std::exchange(v.m_data, nullptr);
    m_cursor = std::exchange(v.m_cursor, 0);
    m_capacity = std::exchange(v.m_capacity, 0);
}

iguana::decoder::entropy_buffer& iguana::decoder::entropy_buffer::operator =(entropy_buffer&& v) {
    if (this != &v) {
        std::uint8_t* const p = std::exchange(m_data, std::exchange(v.m_data, nullptr));
        m_cursor = std::exchange(v.m_cursor, 0);
        m_capacity = std::exchange(v.m_capacity, 0);
        release_memory(p);
    }
    return *this;
}

void iguana::decoder::entropy_buffer::reset(std::size_t n) {
    m_cursor = 0;

    if (n > capacity()) {
        release_memory(std::exchange(m_data, nullptr));
        const auto r = acquire_memory(n);
        m_data = r.first;
        m_capacity = r.second;
    }
}

std::pair<std::uint8_t*, std::size_t> iguana::decoder::entropy_buffer::acquire_memory(std::size_t n) {
    auto* const p = new std::uint8_t[n];
    return { p, n };
}

void iguana::decoder::entropy_buffer::release_memory(std::uint8_t* p) {
    if (p != nullptr) {
        delete[] p;   
    }
}
