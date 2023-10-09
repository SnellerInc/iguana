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
#include <vector>

namespace iguana {

    class byte_buffer {
        using container_type = std::vector<std::uint8_t>;

    public:
        using size_type         = typename container_type::size_type;
        using value_type        = typename container_type::value_type;
        using pointer           = typename container_type::pointer;
        using const_pointer     = typename container_type::const_pointer;
        using reference         = typename container_type::reference;
        using const_reference   = typename container_type::const_reference;
        using iterator          = typename container_type::iterator;
        using const_iterator    = typename container_type::const_iterator;

    private:
         container_type m_data;

    public:
        byte_buffer() = default;
        ~byte_buffer() noexcept = default;
        byte_buffer(const byte_buffer&) = default;
        byte_buffer& operator =(const byte_buffer&) = default;
        byte_buffer(byte_buffer&&) = default;
        byte_buffer& operator =(byte_buffer&&) = default;

    public:

        constexpr size_type size() const noexcept {
            return m_data.size();
        }
    
        constexpr const_pointer data() const noexcept {
            return m_data.data();
        }
        
        constexpr const_reference operator [](size_type k) const noexcept {
            assert(k < size());
            return m_data[k];
        }

        constexpr reference operator [](size_type k) noexcept {
            assert(k < size());
            return m_data[k];
        }

        const_iterator cbegin() const noexcept {
            return m_data.cbegin();
        }

        const_iterator cend() const noexcept {
            return m_data.cend();
        }

        iterator begin() noexcept {
            return m_data.begin();
        }

        const_iterator begin() const noexcept {
            return m_data.begin();
        }

        iterator end() noexcept {
            return m_data.end();
        }

        const_iterator end() const noexcept {
            return m_data.cend();
        }

        void reserve(size_type n) {
            m_data.reserve(n);
        }

        void clear() noexcept {
            m_data.clear();
        }
        
        void append(std::uint8_t v) {
            m_data.push_back(v);
        }

        void append(const std::uint8_t *p, std::size_t n);

        void append(const byte_buffer& buf) {
            append(buf.data(), buf.size());
        }

        template <typename T> void append_little_endian(T v);
        template <typename T> void append_big_endian(T v);
    };
}
