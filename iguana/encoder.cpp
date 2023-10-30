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

#include <cstring>
#include <stdexcept>
#include "encoder.h"

//

namespace iguana {
    const internal::initializer<encoder> encoder::g_Initializer;
}

//

iguana::encoding iguana::encoding_from_string(const char* name) {
    if (std::strcmp(name, "iguana") == 0) {
        return encoding::iguana;
    }

    if (std::strcmp(name, "raw") == 0) {
        return encoding::raw;
    }

    throw std::invalid_argument(std::string("unrecognized encoding '") + name + "'");
}

const char* iguana::to_string(encoding e) {
    switch(e) {
        case encoding::iguana:
            return "iguana";

        case encoding::raw:
            return "raw";

        default:
            throw std::invalid_argument("unrecognized encoding value");        
    }
}

//

iguana::encoder::encoder() {}

iguana::encoder::~encoder() {}

void iguana::encoder::at_process_start() {
    printf("iguana::encoder::at_process_start()\n");
}

void iguana::encoder::at_process_end() {
    printf("iguana::encoder::at_process_end()\n");
}

void iguana::encoder::encode(output_stream& dst, const std::uint8_t* p, std::size_t n) {
    const part prt = {
        .m_entropy_mode = iguana::entropy_mode::ans32,
        .m_encoding = iguana::encoding::iguana,
        .m_rejection_threshold = default_rejection_threshold
    };
    encode(dst, prt);   
}

void iguana::encoder::encode(output_stream& dst, const part* p_parts, std::size_t n_parts) {
    // Compute the total input size
    {   auto total_input_size = std::uint64_t(0);
        for(std::size_t i = 0; i != n_parts; ++i) {
            total_input_size += p_parts->m_size;   
        }
        append_control_var_uint(dst, total_input_size);
    }

/*
	for _, req := range reqs {
		srcLen := len(req.Src)
		if srcLen < 0 {
			return nil, fmt.Errorf("invalid input size %d", srcLen)
		} else if srcLen == 0 {
			continue
		}
		switch req.EncMode {

		case EncodingRaw:
			switch req.EntMode {
			case EntropyNone:
				if err := es.encodeRaw(req.Src); err != nil {
					return nil, err
				}
			case EntropyANS32:
				if err := es.encodeANS32(&req); err != nil {
					return nil, err
				}
			case EntropyANS1:
				if err := es.encodeANS1(&req); err != nil {
					return nil, err
				}
			case EntropyANSNibble:
				if err := es.encodeANSNibble(&req); err != nil {
					return nil, err
				}
			default:
				return nil, fmt.Errorf("unrecognized entropy mode %02x", req.EntMode)
			}

		case EncodingIguana:
			if err := es.encodeIguana(&req); err != nil {
				return nil, err
			}

		default:
			return nil, fmt.Errorf("unrecognized encoding mode %02x", req.EncMode)
		}
	}

	// Append the control bytes in reverse order

	for i := len(es.ctrl) - 1; i >= 0; i-- {
		es.dst = append(es.dst, es.ctrl[i])
	}
	return es.dst, nil
    */
}

void iguana::encoder::append_control_var_uint(output_stream& dst, std::uint64_t v) {
    IGUANA_UNIMPLEMENTED
}

/* TODO:
iguana::encoder::part_ptr iguana::encoder::encode_part(const std::uint8_t* p, std::size_t n) {
    IGUANA_UNIMPLEMENTED
}
*/
