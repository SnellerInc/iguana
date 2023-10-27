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

#include <stdexcept>
#include <cstring>
#include "entropy.h"

//

iguana::entropy_mode iguana::entropy_mode_from_string(const char* name) {
    if (std::strcmp(name, "ans32") == 0) {
        return entropy_mode::ans32;
    }

    if (std::strcmp(name, "ans1") == 0) {
        return entropy_mode::ans1;
    }

    if (std::strcmp(name, "ans_nibble") == 0) {
        return entropy_mode::ans_nibble;
    }

    if (std::strcmp(name, "none") == 0) {
        return entropy_mode::none;
    }

    throw std::invalid_argument(std::string("unrecognized entropy mode '") + name + "'");
}

const char* iguana::to_string(entropy_mode m) {
    switch(m) {
        case entropy_mode::ans32:
            return "ans32";

        case entropy_mode::ans1:
            return "ans1";

        case entropy_mode::ans_nibble:
            return "ans_nibble";

        case entropy_mode::none:
            return "none";

        default:
            throw std::invalid_argument("unrecognized entropy mode value");        
    }
}
