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
#include <vector>
#include "common.h"
#include "span.h"

namespace iguana {
    class IGUANA_API output_stream final {
    public:
        using value_type = std::uint8_t;
        using size_type  = std::size_t;

    private:
        std::vector<value_type> m_content;

    public:
        output_stream() noexcept = default;
        ~output_stream() noexcept = default;

        output_stream(const output_stream&) = delete;
        output_stream& operator =(const output_stream&) = delete;

        output_stream(output_stream&&) = default;
        output_stream& operator =(output_stream&&) = default;

    public:
        size_type size() const noexcept {
            return m_content.size();
        }

        const value_type* data() const noexcept {
            return m_content.data();
        }

        void reserve(size_type n) {
            m_content.reserve(n);
        }

        void reserve_more(size_type n) {
            reserve(size() + n);
        }

        void clear() {
            m_content.clear();
        }

        //

        void append(value_type v) {
            m_content.push_back(v);
        }

        void append(const value_type* p, size_type n);
        void append(const output_stream& s);

        void append(const const_byte_span& s) {
            append(s.data(), s.size());
        }

        template <
            typename T
        > void append(T first, T last) {
            while(first != last) {
                append(*first++);
            }
        }

        //

        void append_reverse(const value_type* p, size_type n);
        void append_reverse(const output_stream& s);

        void append_reverse(const const_byte_span& s) {
            append_reverse(s.data(), s.size());
        }

        //

        template <
            typename T
        > std::enable_if_t<std::is_unsigned_v<T>> append_little_endian(T v);

        template <
            typename T
        > std::enable_if_t<std::is_unsigned_v<T>> append_big_endian(T v);
    };

    //

    template <
        typename T
    > inline std::enable_if_t<std::is_unsigned_v<T>> output_stream::append_little_endian(T v) {
        // TODO: optimize me
        for(std::size_t i = 0; i != sizeof(T); ++i) {
            append(static_cast<value_type>(v >> (i * 8)));
        }
    }

    template <
        typename T
    > inline std::enable_if_t<std::is_unsigned_v<T>> output_stream::append_big_endian(T v) {
        // TODO: optimize me
        for(std::size_t i = 0; i != sizeof(T); ++i) {
            append(static_cast<value_type>(v >> ((sizeof(T) - 1 - i) * 8)));
        }
    }
}
