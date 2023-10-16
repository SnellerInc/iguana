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
#include "ans_statistics.h"
#include "input_stream.h"
#include "output_stream.h"

namespace iguana::ans {
    class IGUANA_API encoder {
    protected:
        encoder() noexcept {}

    public:
        virtual ~encoder() noexcept;

    public:
        encoder(const encoder&) = delete;
        encoder& operator =(const encoder&) = delete;

        encoder(encoder&& v) = default;
        encoder& operator =(encoder&& v) = default;

    public:
        virtual void encode(output_stream& dst, const statistics& stats, const std::uint8_t *src, std::size_t src_len) = 0;
        void encode(output_stream& dst, const std::uint8_t *src, std::size_t src_len);

        void encode(output_stream& dst, const ans::statistics& stats, const const_byte_span& src) {
            encode(dst, stats, src.data(), src.size());
        }

        void encode(output_stream& dst, const const_byte_span& src) {
            encode(dst, src.data(), src.size());
        }
    };
}
