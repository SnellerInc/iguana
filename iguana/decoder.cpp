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

#include "decoder.h"
#include "command.h"

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
}

//

iguana::decoder::~decoder() noexcept {}

void iguana::decoder::decode(output_stream& dst, input_stream& src) {
    std::size_t cursor = src.size();
    if (cursor == 0) {
        throw out_of_input_data_exception();
    }

    --cursor;
	const auto uncompressed_len = read_control_var_uint(src, cursor);

	if (uncompressed_len == 0) {
		return;
	}

    dst.reserve_more(uncompressed_len);

/*TODO
	dst, ec = d.decode(uncompressedLen, cursor, dst, src)
	if ec != ecOK {
		return dst, errs[ec]
	}
*/
}

std::uint64_t iguana::decoder::read_control_var_uint(const std::uint8_t* src, std::size_t& cursor) {

}

void iguana::decoder::decode(output_stream& dst, const std::uint8_t* const src, std::uint64_t uncompressed_len, std::size_t ctrl_cursor) {

/*
func (d *Decoder) decode(, dst []byte, src []byte) ([]byte, errorCode) {
	d.reset()
	var ec errorCode
*/
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
                const std::uint8_t * const ans = src + data_cursor;

/*TODO

                ans := src[dataCursor : dataCursor+lenCompressed]
                encoded, ec := ansDecodeTable(&d.anstab, ans)
                if ec != ecOK {
                    return dst, ec
                }
                dst, ec = ans32DecodeExplicit(encoded, &d.anstab, int(lenUncompressed), dst)
                if ec != ecOK {
                    return dst, ec
                }*/
                data_cursor += len_compressed;
            } break;

		case command::decode_ans1: {
                const std::uint64_t len_uncompressed = read_control_var_uint(src, ctrl_cursor);
                const std::uint64_t len_compressed = read_control_var_uint(src, ctrl_cursor);
                const std::uint8_t * const ans = src + data_cursor;
/*TODO
			ans := src[dataCursor : dataCursor+lenCompressed]
			encoded, ec := ansDecodeTable(&d.anstab, ans)
			if ec != ecOK {
				return dst, ec
			}
			dst, ec = ans1DecodeExplicit(encoded, &d.anstab, int(lenUncompressed), dst)
			if ec != ecOK {
				return dst, ec
			}*/

    			data_cursor += len_compressed;
            } break;
/*
		case cmdDecodeANSNibble:
			var lenUncompressed, lenCompressed uint64
			lenUncompressed, ctrlCursor, ec = readControlVarUint(src, ctrlCursor)
			if ec != ecOK {
				return dst, ec
			}
			lenCompressed, ctrlCursor, ec = readControlVarUint(src, ctrlCursor)
			if ec != ecOK {
				return dst, ec
			}
			ans := src[dataCursor : dataCursor+lenCompressed]
			encoded, ec := ansNibbleDecodeTable(&d.ansnibtab, ans)
			if ec != ecOK {
				return dst, ec
			}
			dst, ec = ansNibbleDecodeExplicit(encoded, &d.ansnibtab, int(lenUncompressed), dst)
			if ec != ecOK {
				return dst, ec
			}
			dataCursor += lenCompressed

		case cmdDecodeIguana:
			// Fetch the header byte
			if ctrlCursor < 0 {
				return dst, ecOutOfInputData
			}

			var hdr uint64
			hdr, ctrlCursor, ec = readControlVarUint(src, ctrlCursor)
			if ec != ecOK {
				return dst, ec
			}

			// Fetch the uncompressed streams' lengths
			if hdr == 0 {
				for i := stridType(0); i < streamCount; i++ {
					var uLen uint64
					uLen, ctrlCursor, ec = readControlVarUint(src, ctrlCursor)
					if ec != ecOK {
						return dst, ec
					}
					d.pack[i].data = src[dataCursor : dataCursor+uLen]
					dataCursor += uLen
				}
			} else {
				var ulens [streamCount]uint64
				entropyBufferSize := uint64(0)

				for i := stridType(0); i < streamCount; i++ {
					var uLen uint64
					uLen, ctrlCursor, ec = readControlVarUint(src, ctrlCursor)
					if ec != ecOK {
						return dst, ec
					}
					ulens[i] = uLen
					if entropyMode := EntropyMode((hdr >> (i * 4)) & 0x0f); entropyMode != EntropyNone {
						entropyBufferSize += uLen
					}
				}
				if uint64(cap(d.entbuf)) < entropyBufferSize+padSize {
					// ensure the output is appropriately padded:
					d.entbuf = make([]byte, entropyBufferSize, entropyBufferSize+padSize)
				}
				entOffs := uint64(0)

				for i := stridType(0); i < streamCount; i++ {
					uLen := ulens[i]
					if entropyMode := EntropyMode((hdr >> (i * 4)) & 0x0f); entropyMode == EntropyNone {
						d.pack[i].data = d.padStream(i, src[dataCursor:dataCursor+uLen])
						dataCursor += uLen
					} else {
						var cLen uint64
						cLen, ctrlCursor, ec = readControlVarUint(src, ctrlCursor)
						if ec != ecOK {
							return dst, ec
						}
						switch entropyMode {
						case EntropyANS32:
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
							entOffs += uLen

						case EntropyANS1:
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
							entOffs += uLen

						case EntropyANSNibble:
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

						default:
							panic("unrecognized entropy mode")
						}
					}
				}
			}

			d.lastOffs = -initLastOffset
			dst, ec = decompressIguana(dst, &d.pack, &d.lastOffs)

			if ec != ecOK {
				return dst, ec
			}
*/
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

void iguana::decoder::at_process_start() {
    printf("iguana::decoder::at_process_start()\n");
}

void iguana::decoder::at_process_end() {
    printf("iguana::decoder::at_process_end()\n");
}
