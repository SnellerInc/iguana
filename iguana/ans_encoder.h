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
#include "buffer.h"
#include "ans_statistics.h"

namespace iguana::ans {
    class iguana_public encoder {
    protected:
        byte_buffer m_buf;
        
    protected:
        encoder();
        virtual ~encoder() noexcept;

    public:
        encoder(const encoder&) = delete;
        encoder& operator =(const encoder&) = delete;

        encoder(encoder&& v) = default;
        encoder& operator =(encoder&& v) = default;
        
    public:
        virtual error_code encode(const std::uint8_t *src, std::size_t n, const statistics& stats) = 0;
        error_code encode(const std::uint8_t *src, std::size_t n);

        error_code encode(const const_byte_span& src, const ans::statistics& stats) {
            return encode(src.data(), src.size(), stats);
        }

        error_code encode(const const_byte_span& src) {
            return encode(src.data(), src.size());
        }

        virtual void clear() = 0;
    };
}
