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
    // entropy_mode specifies what entropy compression should be applied to the results of the selected structural compressor.
    enum class entropy_mode : std::uint8_t {
        none    = 0x00,     // No entropy compression is applied
        ans32   = 0x01,     // Vectorized, 32-way interleaved 8-bit rANS entropy compression should be applied
        ans1    = 0x02,     // Scalar, one-way 8-bit rANS entropy compression should be applied
        ans_nibble = 0x03   // Scalar, one-way 4-bit rANS entropy compression should be applied
    };  
}
