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

//

#include "platform.h"

//

enum iguana_error_code {
    ok = 0,
    corrupted_bitstream,
    wrong_source_size,
    out_of_input_data,
    insufficient_target_capacity,
    unrecognized_command,
    out_of_memory
};

//

IGUANA_API const char* iguana_get_error_description(iguana_error_code ec);

//

IGUANA_API iguana_error_code iguana_compress(const uint8_t* p, size_t n);
