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

#include "c_bindings.h"
#include "error.h"

//

const char* iguana_get_error_description(iguana_error_code ec) {
    return iguana::get_error_description(static_cast<iguana::error_code>(ec));
}

//

iguana_error_code iguana_compress(const uint8_t* p, size_t n) try {
    // TODO    

} catch(const iguana::exception& e) {
    return static_cast<iguana_error_code>(e.get_error_code());
} catch(...) {
    return corrupted_bitstream;
}
