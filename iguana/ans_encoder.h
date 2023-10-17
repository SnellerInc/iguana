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
    > class basic_encoder {
    public:
        using statistics = T_STATISTICS;

    protected:
        basic_encoder() noexcept {}
        ~basic_encoder() noexcept {}

    public:
        basic_encoder(const basic_encoder&) = delete;
        basic_encoder& operator =(const basic_encoder&) = delete;

        basic_encoder(basic_encoder&& v) = default;
        basic_encoder& operator =(basic_encoder&& v) = default;

    public:
        void encode(output_stream& dst, const std::uint8_t *src, std::size_t src_len);

        void encode(output_stream& dst, const statistics& stats, const const_byte_span& src) {
            T::encode(dst, stats, src.data(), src.size());
        }

        void encode(output_stream& dst, const const_byte_span& src) {
            encode(dst, src.data(), src.size());
        }
    };

    //

    template <
        typename T_CONCRETE,
        typename T_STATISTICS
    > void basic_encoder <
        T_CONCRETE,
        T_STATISTICS
    >::encode(output_stream& dst, const std::uint8_t *src, std::size_t src_len) {
        T::encode(dst, statistics(src, src_len), src, src_len);
    }
}
