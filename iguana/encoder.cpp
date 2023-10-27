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

iguana::encoder::part_ptr iguana::encoder::encode_part(const std::uint8_t* p, std::size_t n) {
    IGUANA_UNIMPLEMENTED
}

void iguana::encoder::at_process_start() {
    printf("iguana::encoder::at_process_start()\n");
}

void iguana::encoder::at_process_end() {
    printf("iguana::encoder::at_process_end()\n");
}
