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

namespace iguana::utils {

    template <
        typename T
    > struct invalid final {};

    //

    template <
        typename T1,
        typename T2
    > constexpr inline bool is_aligned(T1 x, T2 y) noexcept {
        return (x % y) == 0;
    }

    template <
        typename T1,
        typename T2
    > constexpr inline auto align_down(T1 x, T2 y) noexcept {
        return (x / y) * y;
    }

    template <
        typename T1,
        typename T2
    > constexpr inline auto align_up(T1 x, T2 y) noexcept {
        return ((x + y - 1) / y) * y;
    }

    //

    template <
        typename T
    > inline std::enable_if_t<std::is_integral_v<T> && sizeof(T) == 1, T> swap_bytes(T v) noexcept {
        return v;
    }

    template <
        typename T
    > inline std::enable_if_t<std::is_integral_v<T> && sizeof(T) == 2, T> swap_bytes(T v) noexcept {
        return __builtin_bswap16(v);
    }

    template <
        typename T
    > inline std::enable_if_t<std::is_integral_v<T> && sizeof(T) == 4, T> swap_bytes(T v) noexcept {
        return __builtin_bswap32(v);
    }

    template <
        typename T
    > inline std::enable_if_t<std::is_integral_v<T> && sizeof(T) == 8, T> swap_bytes(T v) noexcept {
        return __builtin_bswap64(v);
    }

    //

    template <
        typename T
    > inline std::enable_if_t<std::is_integral_v<T>, T> read_little_endian(const void* p) noexcept {
    #if IGUANA_PROCESSOR_LITTLE_ENDIAN
        return *static_cast<const T*>(p);
    #else
        return swap_bytes(*static_cast<const T*>(p));
    #endif
    }

    template <
        typename T
    > inline std::enable_if_t<std::is_integral_v<T>, T> read_big_endian(const void* p) noexcept {
    #if IGUANA_PROCESSOR_LITTLE_ENDIAN
        return swap_bytes(*static_cast<const T*>(p));
    #else
        return *static_cast<const T*>(p);
    #endif
    }

    //

    template <
        typename T
    > inline std::enable_if_t<std::is_integral_v<T>> write_little_endian(void* p, T v) noexcept {
    #if IGUANA_PROCESSOR_LITTLE_ENDIAN
        *static_cast<const T*>(p) = v;
    #else
        *static_cast<const T*>(p) = swap_bytes(v);
    #endif
    }

    template <
        typename T
    > inline std::enable_if_t<std::is_integral_v<T>> write_big_endian(void* p, T v) noexcept {
    #if IGUANA_PROCESSOR_LITTLE_ENDIAN
        *static_cast<const T*>(p) = swap_bytes(v);
    #else
        *static_cast<const T*>(p) = v;
    #endif
    }
}
