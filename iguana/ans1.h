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

namespace iguana::ans1 {

    class iguana_public encoder {
    public:
        constexpr inline static std::size_t   word_l_bits = 16;
        constexpr inline static std::uint32_t word_l = std::uint32_t(1) << word_l_bits;
        constexpr inline static std::size_t   word_m_bits = 12;
        constexpr inline static std::uint32_t word_m = std::uint32_t(1) << word_m_bits;

    private:
	    std::uint32_t       m_state = word_l;
        byte_span      	    m_src;
        byte_buffer         m_buf;
        ans::statistics*    m_stats = nullptr;
        
    public:
        encoder(const byte_span& src, ans::statistics* p_stats)
          : m_state(word_l)
          , m_src(src)
          , m_buf()
          , m_stats(p_stats) {}

        ~encoder() noexcept = default;

        encoder(const encoder&) = delete;
        encoder& operator =(const encoder&) = delete;

        encoder(encoder&&) = default;
        encoder& operator =(encoder&&) = default;

    public:


    private:
        void put(std::uint8_t v);
        void flush();
    };

    class iguana_public decoder {

    };
}
