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
#include <cstring>
#include <array>
#include "common.h"

namespace iguana::memory {
    namespace internal {
        inline void zero(void* p, std::size_t n) noexcept {
            std::memset(p, 0, n); // TODO: optimize me!
        }
    }

    template <
        typename T
    > inline std::enable_if_t<std::is_trivial_v<T>> zero(T* p, std::size_t n) noexcept {
        internal::zero(static_cast<void*>(p), n * sizeof(T));
    }

    template <
        typename T,
        std::size_t N
    > inline void zero(T (&a)[N]) noexcept {
        zero(&a[0], N);        
    }

    template <
        typename T,
        std::size_t N
    > inline void zero(std::array<T, N>& a) noexcept {
        zero(a.data(), N);        
    }

    template <
        typename T
    > inline std::enable_if_t<std::is_trivial_v<T>> fill(T* p, std::size_t n, const T& v) noexcept {
        for(std::size_t i = 0; i < n; ++i) {
            p[i] = v;
        }
    }

    template <
        typename T,
        std::size_t N
    > inline void fill(T (&a)[N], const T& v) noexcept {
        fill(&a[0], N, v);        
    }

    template <
        typename T,
        std::size_t N
    > inline void fill(std::array<T, N>& a, const T& v) noexcept {
        fill(a.data(), N, v);        
    }
}
