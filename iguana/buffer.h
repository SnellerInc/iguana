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
        using size_type = typename container_type::size_type;

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
    
        void reserve(size_type n) {
            m_data.reserve(n);
        }
        
        template <typename T> void append_little_endian(T v);
    };
}
