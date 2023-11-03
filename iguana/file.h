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
#include <cstdio>
#include <utility>
#include <string>
#include "common.h"

//

namespace iguana {

    class file final {
        std::FILE* m_file = nullptr;
        bool       m_close = false;

    public:
        file() noexcept {}
        file(const std::string& name, const std::string& mode);

        file(std::FILE* p, bool should_close) noexcept
          : m_file(p)
          , m_close(should_close) {}

        ~file() noexcept {
            close();
        }

        file(const file&) = delete;   
        file& operator =(const file&) = delete;   

        file(file&& v)
          : m_file(std::exchange(v.m_file, nullptr))
          , m_close(std::exchange(v.m_close, false)) {}   

        file& operator =(file&& v);

    public:
        void close() noexcept;
        void reset(std::FILE* p, bool should_close) noexcept;
        void open(const std::string& name, const std::string& mode);   
        void read(void* __restrict__ p, std::uint64_t n);
        std::uint64_t size() const;

        constexpr std::FILE* get() const noexcept {
            return m_file;
        }
        
        constexpr bool valid() const noexcept {
            return m_file != nullptr;
        }

        constexpr bool operator !() const noexcept {
            return !valid();
        }

        constexpr explicit operator bool() const noexcept {
            return valid(); 
        }

        constexpr operator std::FILE*() const noexcept {
            return get();
        }
    };
}
