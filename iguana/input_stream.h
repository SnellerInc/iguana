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

namespace iguana {
    class iguana_public input_stream {
        const std::uint8_t* m_start = nullptr;
        const std::uint8_t* m_end   = nullptr;

    public:
        input_stream() noexcept = default;            

        input_stream(const std::uint8_t* s, const std::uint8_t* e) noexcept 
          : m_start(s)
          , m_end(e) {}       

        input_stream(const std::uint8_t* s, std::size_t n) noexcept 
          : m_start(s)
          , m_end(s + n) {}      

        ~input_stream() noexcept = default;

        input_stream(const input_stream&) = default;
        input_stream& operator =(const input_stream&) = default;

        input_stream(input_stream&&) = default;
        input_stream& operator =(input_stream&&) = default;

    public:

        bool empty() const noexcept {
            return m_start == m_end;
        }

        std::size_t size() const noexcept {
            return m_end - m_start;
        }

        const std::uint8_t* data() const noexcept {
            return m_start;
        }

        const std::uint8_t* edata() const noexcept {
            return m_end;
        }

        void consume_from_start(std::size_t n) noexcept {
            m_start += n;
        }

        void consume_from_end(std::size_t n) noexcept {
            m_end -= n;
        }

        void set_start(const std::uint8_t* p) noexcept {
            m_start = p;
        }

        void set_end(const std::uint8_t* p) noexcept {
            m_end = p;
        }

        std::uint8_t operator [](std::size_t idx) const noexcept {
            assert(idx < size());
            return m_start[idx];
        }
    };
}
