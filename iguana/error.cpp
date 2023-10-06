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

const char* iguana::get_error_description(error_code ec) {
    switch(ec) {
    case error_code::ok:
        return "success";

    case error_code::corrupted_bitstream:
        return "bitstream corruption detected";

    case error_code::wrong_source_size:
        return "wrong source size";

	case error_code::out_of_input_data:
        return "out of input bytes";

	case error_code::unrecognized_command:
        return "unrecognized command";

	case error_code::insufficient_target_capacity:
        return "insufficient target capacity";
    };
}
