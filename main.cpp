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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <stdexcept>
#include <iostream>
#include "iguana/error.h"

// TODO: use a proper makefile
#include "iguana/ans_byte_statistics.cpp"
#include "iguana/ans_nibble_statistics.cpp"
#include "iguana/ans1.cpp"
#include "iguana/ans32.cpp"
#include "iguana/ans_nibble.cpp"
#include "iguana/ans_bitstream.cpp"
#include "iguana/error.cpp"
#include "iguana/output_stream.cpp"
#include "iguana/decoder.cpp"
#include "iguana/encoder.cpp"
#include "iguana/c_bindings.cpp"

//

namespace iguana {
    
}

//

int main(int argc, char *argv[]) try {
    printf("argc = %d\n", argc); 
/*
    for(int i = 1; i < argc; ++i) {
        const char* const opt = argv[i];

        if (std::strcmp(opt, "-d") == 0) {
            if (
        }
    }
*/
    //printf("path = %s\n");

    throw std::invalid_argument("jeblo");

 //   printf("Hello, world\n");
    return EXIT_SUCCESS;

} catch(const std::exception& e) {
    std::cerr << "exception: " << e.what() << std::endl;
    return EXIT_FAILURE;

} catch(...) {
    std::cerr << "unrecognized exception" << std::endl;
    return EXIT_FAILURE;
}
