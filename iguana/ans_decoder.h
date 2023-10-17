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
#include "common.h"
#include "span.h"
#include "input_stream.h"
#include "output_stream.h"

namespace iguana::ans {

    template <
        typename T_CONCRETE,
        typename T_STATISTICS
    > class basic_decoder {
    public:
        using statistics = T_STATISTICS;

    protected:
        basic_decoder() {}
        ~basic_decoder() noexcept {}

    public:
        basic_decoder(const basic_decoder&) = delete;
        basic_decoder& operator =(const basic_decoder&) = delete;

        basic_decoder(basic_decoder&& v) = default;
        basic_decoder& operator =(basic_decoder&& v) = default;

    public:
        void decode(output_stream& dst, std::size_t result_size, input_stream& src);
    };

    //

    template <
        typename T_CONCRETE,
        typename T_STATISTICS
    > void basic_decoder <
        T_CONCRETE,
        T_STATISTICS
    >::decode(output_stream& dst, std::size_t result_size, input_stream& src) {
        const typename T::statistics stats(src);
        statistics::decoding_table tab;
        stats.build_decoding_table(tab);
        T::decode(dst, result_size, src, tab);
    }
}
