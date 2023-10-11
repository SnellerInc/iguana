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

#include "ans_decoder.h"

iguana::ans::decoder::~decoder() noexcept {}

void iguana::ans::decoder::decode(output_stream& dst, std::size_t result_size, input_stream& src) {
    const statistics stats(src);
    statistics::decoding_table tab;
    stats.build_decoding_table(tab);
    decode(dst, result_size, src, tab);
}        
