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

#pragma once
#include "common.h"

namespace iguana {
    enum class command : std::uint8_t {
	    copy_raw = 0x00,
	    decode_iguana = 0x01,
	    decode_ans32 = 0x02,
	    decode_ans1 = 0x03,
	    decode_ans_nibble = 0x04,
    };

    //

	static constexpr const std::uint8_t last_command_marker = 0x80;
	static constexpr const std::uint8_t command_mask = last_command_marker ^ 0xff;
}
