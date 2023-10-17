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

#include "ans_nibble.h"

namespace iguana::ans_nibble {
    void (*encoder::g_Compress)(context& ctx) = &encoder::compress_portable;
    const internal::initializer<encoder> encoder::g_Initializer;

    void (*decoder::g_Decompress)(context& ctx) = &decoder::decompress_portable;
    const internal::initializer<decoder> decoder::g_Initializer;
}

iguana::ans_nibble::encoder::~encoder() noexcept {}

void iguana::ans_nibble::encoder::at_process_start() {
    printf("iguana::ans_nibble::encoder::at_process_start()\n");
}

void iguana::ans_nibble::encoder::at_process_end() {
    printf("iguana::ans_nibble::encoder::at_process_end()\n");
}

iguana::ans_nibble::decoder::~decoder() noexcept {}

void iguana::ans_nibble::decoder::at_process_start() {
    printf("iguana::ans_nibble::decoder::at_process_start()\n");
}

void iguana::ans_nibble::decoder::at_process_end() {
    printf("iguana::ans_nibble::decoder::at_process_end()\n");
}
