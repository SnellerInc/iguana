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

#include "decoder.h"

//

namespace iguana {
    void (*decoder::g_Decompress)(context& ctx) = &decoder::decompress_portable;
    const internal::initializer<decoder> decoder::g_Initializer;
}

//

iguana::decoder::~decoder() noexcept {}

void iguana::decoder::decompress_portable(context& ctx) {
    
}

void iguana::decoder::at_process_start() {
    printf("iguana::decoder::at_process_start()\n");
}

void iguana::decoder::at_process_end() {
    printf("iguana::decoder::at_process_end()\n");
}
