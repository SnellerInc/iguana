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

#include "error.h"
#include <array>
#include <stdexcept>

namespace iguana {
    namespace {
        const std::array<const char*, 7> g_ErrorDescription = {
            "success",
            "bitstream corruption detected",
            "wrong source size",
            "out of input bytes",
            "unrecognized command",
            "insufficient target capacity",
            "out of memory"
        };
    }
}

const char* iguana::get_error_description(error_code ec) {
    const auto idx = static_cast<std::size_t>(ec);
    assert(idx < g_ErrorDescription.size());
    return g_ErrorDescription[idx];
}

iguana::exception::~exception() {}

void iguana::exception::from_error(error_code ec) {
    switch(ec) {
        case error_code::corrupted_bitstream:
            throw corrupted_bitstream_exception();

        case error_code::wrong_source_size:
            throw wrong_source_size_exception();

        case error_code::out_of_input_data:
            throw out_of_input_data_exception();

        case error_code::insufficient_target_capacity:
            throw insufficient_target_capacity_exception();

        case error_code::unrecognized_command:
            throw unrecognized_command_exception();

        case error_code::out_of_memory:
            throw out_of_memory_exception();

        case error_code::ok:
            throw std::invalid_argument("unrecognized error code");
    }
}

iguana::corrupted_bitstream_exception::~corrupted_bitstream_exception() {}

iguana::error_code iguana::corrupted_bitstream_exception::get_error_code() const noexcept {
    return error_code::corrupted_bitstream;
}

iguana::wrong_source_size_exception::~wrong_source_size_exception() {}

iguana::error_code iguana::wrong_source_size_exception::get_error_code() const noexcept {
    return error_code::wrong_source_size;
}

iguana::out_of_input_data_exception::~out_of_input_data_exception() {}

iguana::error_code iguana::out_of_input_data_exception::get_error_code() const noexcept {
    return error_code::out_of_input_data;
}

iguana::insufficient_target_capacity_exception::~insufficient_target_capacity_exception() {}

iguana::error_code iguana::insufficient_target_capacity_exception::get_error_code() const noexcept {
    return error_code::insufficient_target_capacity;
}

iguana::unrecognized_command_exception::~unrecognized_command_exception() {}

iguana::error_code iguana::unrecognized_command_exception::get_error_code() const noexcept {
    return error_code::unrecognized_command;
}

iguana::out_of_memory_exception::~out_of_memory_exception() {}

iguana::error_code iguana::out_of_memory_exception::get_error_code() const noexcept {
    return error_code::out_of_memory;
}
