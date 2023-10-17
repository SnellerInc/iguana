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
#include "input_stream.h"
#include "output_stream.h"

namespace iguana::ans {

    class IGUANA_API nibble_statistics {
        class builder;

    public:
        constexpr inline static std::size_t   word_M_bits = 12;
        constexpr inline static std::size_t   word_L_bits = 16;
        constexpr inline static std::uint32_t word_L = std::uint32_t(1) << word_L_bits;
        constexpr inline static std::uint32_t word_M = std::uint32_t(1) << word_M_bits;

        //

        constexpr inline static std::uint32_t frequency_bits = word_M_bits;
        constexpr inline static std::uint32_t frequency_mask = (1 << frequency_bits) - 1;
        constexpr inline static std::uint32_t cumulative_frequency_bits = word_M_bits;
        constexpr inline static std::uint32_t cumulative_frequency_mask = (1 << cumulative_frequency_bits) - 1;

        //

        constexpr inline static std::size_t ctrl_block_size         = 6;
        constexpr inline static std::size_t nibble_block_max_length = 24; // 16 3-nibble groups
        constexpr inline static std::size_t dense_table_max_length  = ctrl_block_size + nibble_block_max_length;

        //

        constexpr inline static std::size_t initial_buffer_size = 1 << 20;

    public:
        using decoding_table = std::uint32_t[word_M];

    private:
        std::array<std::uint32_t, 16> m_table;

    public:
        nibble_statistics() noexcept {
            memory::zero(m_table);
        }

        explicit nibble_statistics(const_byte_span s) noexcept
          : nibble_statistics(s.data(), s.size()) {}

        nibble_statistics(const std::uint8_t *p, std::size_t n) noexcept
          : nibble_statistics() {
            compute(p, n);
        }

        explicit nibble_statistics(input_stream& s) {
            deserialize(s);
        }

        ~nibble_statistics() = default;
        nibble_statistics(const nibble_statistics&) = default;
        nibble_statistics& operator =(const nibble_statistics&) = default;
        nibble_statistics(nibble_statistics&&) = default;
        nibble_statistics& operator =(nibble_statistics&&) = default;

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

        void build_decoding_table(decoding_table& tab) const noexcept;

    public:
        void serialize(output_stream& s) const;
        void deserialize(input_stream& s);

    private:
        static std::uint32_t fetch_nibble(input_stream& s, ssize_t& nibidx);
    };
}
