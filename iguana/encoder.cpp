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
#include <numeric>
#include "encoder.h"
#include "bitops.h"
#include "error.h"

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

void iguana::encoder::encode(output_stream& dst, const part& p) {
    append_control_var_uint(p.m_size);
    encode_part(dst, p);

	// Append the control bytes in reverse order
    dst.append_reverse(m_control.data(), m_control.size());
    m_control.clear();
}

void iguana::encoder::encode(output_stream& dst, const part* first, const part* last) {
    // Compute the total input size
    {   const auto total_input_size = std::accumulate(
            first,
            last,
            std::uint64_t(0),
            [](std::uint64_t lhs, const part& rhs) {
                return lhs + rhs.m_size;
            }
        );
        append_control_var_uint(total_input_size);
    }

    for(const auto* i = first; i != last; ++i) {
        encode_part(dst, *i);
    }

	// Append the control bytes in reverse order
    dst.append_reverse(m_control.data(), m_control.size());
    m_control.clear();
}

void iguana::encoder::encode_part(output_stream& dst, const part& p) {
    if (SELDOM(p.m_size == 0)) {
        return;
    }

    switch(p.m_encoding) {
    case encoding::raw:
        switch(p.m_entropy_mode) {
        case entropy_mode::none:
            encode_raw(dst, p);
            break;

        case entropy_mode::ans32:
            encode_ans32(dst, p);
            break;

        case entropy_mode::ans1:
            encode_ans1(dst, p);
            break;

        case entropy_mode::ans_nibble:
            encode_ans_nibble(dst, p);
            break;

        default:
            throw std::invalid_argument(std::string("unrecognized entropy mode '") + to_string(p.m_entropy_mode) + "'");              
        }
        break;

    case encoding::iguana:
        encode_iguana(dst, p);
        break;

    default:
        throw std::invalid_argument(std::string("unrecognized encoding '") + to_string(p.m_encoding) + "'");
    }
}

void iguana::encoder::append_control_var_uint(std::uint64_t v) {
    const auto cnt = (bit::length(v) / 7) + 1;

	for(auto i = static_cast<int>(cnt) - 1; i >= 0; --i) {
		auto x = static_cast<std::uint8_t>((v >> (i*7)) & 0x7f);
		if (i == 0) {
			x |= 0x80;
		}
        m_control.push_back(x);
	}
}

void iguana::encoder::encode_raw(output_stream& dst, const part& p) {
    IGUANA_UNIMPLEMENTED
}

void iguana::encoder::encode_iguana(output_stream& dst, const part& p) {
    IGUANA_UNIMPLEMENTED
}

void iguana::encoder::encode_ans32(output_stream& dst, const part& p) {
    IGUANA_UNIMPLEMENTED
}

void iguana::encoder::encode_ans1(output_stream& dst, const part& p) {
    IGUANA_UNIMPLEMENTED
}

void iguana::encoder::encode_ans_nibble(output_stream& dst, const part& p) {
    IGUANA_UNIMPLEMENTED
}
