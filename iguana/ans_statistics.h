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
#include <array>
#include "common.h"
#include "span.h"
#include "memops.h"
#include "buffer.h"
#include "ans_histogram.h"

namespace iguana::ans {

    constexpr inline static std::size_t   word_M_bits = 12;
    constexpr inline static std::size_t   word_L_bits = 16;
    constexpr inline static std::uint32_t word_L = std::uint32_t(1) << word_L_bits;
    constexpr inline static std::uint32_t word_M = std::uint32_t(1) << word_M_bits;

    constexpr inline static std::size_t initial_buffer_size = 1 << 20;

    class iguana_public statistics {
        class collector;

    public:
        constexpr inline static std::uint32_t frequency_bits = word_M_bits;
        constexpr inline static std::uint32_t frequency_mask = (1 << frequency_bits) - 1;
        constexpr inline static std::uint32_t cumulative_frequency_bits = word_M_bits;
        constexpr inline static std::uint32_t cumulative_frequency_mask = (1 << cumulative_frequency_bits) - 1;

    public:
        std::array<std::uint32_t, 256> m_table;

    public:   
        statistics() noexcept {
            memory::zero(m_table); 
        }

        explicit statistics(const_byte_span s) noexcept
          : statistics(s.data(), s.size()) {}
        
        statistics(const std::uint8_t *p, std::size_t n) noexcept
          : statistics() {
            compute(p, n);
        }

        ~statistics() = default;      
        statistics(const statistics&) = default;
        statistics& operator =(const statistics&) = default;
        statistics(statistics&&) = default;
        statistics& operator =(statistics&&) = default;

    public:
        std::uint32_t operator [](std::size_t k) const noexcept {
            assert(k < m_table.size());  
            return m_table[k];
        }

    public:
        void compute(const std::uint8_t *p, std::size_t n) noexcept;

        void compute(const_byte_span s) noexcept {
            return compute(s.data(), s.size());
        }

    public:
        void serialize(byte_buffer& buf) const;
    };
}
