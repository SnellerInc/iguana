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

namespace iguana {
    template <
        typename T_DATA,
        typename T_SIZE = std::size_t
    > class span {
    public:
        using value_type        = T_DATA;
        using size_type         = T_SIZE;
        using pointer           = value_type*;
        using const_pointer     = const value_type*;
        using reference         = value_type&;
        using const_reference   = const value_type&;
        using iterator          = pointer;
        using const_iterator    = const_pointer;
        
    public:
        pointer     m_data = nullptr;
        size_type   m_size = 0;
        
    public:
        span() noexcept = default;
        span(const span&) noexcept = default;
        span(span&&) noexcept = default;

        span(pointer p, size_type n) noexcept
          : m_data(p)
          , m_size(n) {
            assert((n != 0) || (p == nullptr));
        }

        ~span() noexcept = default;
   
        span& operator =(const span&) noexcept = default;
        span& operator =(span&&) noexcept = default;
        
    public:
        constexpr size_type size() const noexcept {
            return m_size;
        }

        constexpr pointer data() const noexcept {
            return m_data;
        }
        
        constexpr const_reference operator [](size_type k) const noexcept {
            assert(k < size());
            return m_data[k];
        }

        constexpr reference operator [](size_type k) noexcept {
            assert(k < size());
            return m_data[k];
        }

        constexpr const_iterator cbegin() const noexcept {
            return m_data;
        }

        constexpr const_iterator cend() const noexcept {
            return m_data + m_size;
        }

        constexpr iterator begin() noexcept {
            return m_data;
        }

        constexpr const_iterator begin() const noexcept {
            return cbegin();
        }

        constexpr iterator end() noexcept {
            return m_data + m_size;
        }

        constexpr const_iterator end() const noexcept {
            return cend();
        }
    }; 

    using byte_span = span<std::uint8_t>;
    using const_byte_span = span<const std::uint8_t>;
}
