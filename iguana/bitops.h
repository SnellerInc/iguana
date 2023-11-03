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
#include <cassert>
#if defined(IGUANA_COMPILER_MSVC)
    #include <intrin.h> 

    //

    #pragma intrinsic(__lzcnt)
    #pragma intrinsic(__lzcnt64)
    #pragma intrinsic(_tzcnt_u32)
    #pragma intrinsic(_tzcnt_u64)
    #pragma intrinsic(__popcnt)
    #pragma intrinsic(__popcnt64)
    #pragma intrinsic(_BitScanForward64)
    #pragma intrinsic(_BitScanReverse64)
#endif
#include "common.h"

namespace iguana::bit {

    template <
        typename T
    > constexpr std::enable_if_t<std::is_unsigned_v<T>, bool> is_power_of_2_or_zero(T v) noexcept {
        return (v & (v - 1)) == T(0);
    }

    template <
        typename T
    > constexpr std::enable_if_t<std::is_unsigned_v<T>, bool> is_power_of_2(T v) noexcept {
        return is_power_of_2_or_zero(v) && (v != T(0));
    }

    //

    inline unsigned int find_first_set(unsigned int x) noexcept {
        assert(x != 0);
    #if defined(IGUANA_COMPILER_MSVC)
        unsigned long index = 0;
        _BitScanForward64(&index, static_cast<unsigned __int64>(x));
        return static_cast<unsigned int>(index);
    #elif defined(IGUANA_COMPILER_GNU) || defined(IGUANA_COMPILER_CLANG)
        return static_cast<unsigned int>(__builtin_ffs(x));
    #else
        #error Unsupported compiler
    #endif
    }

    inline unsigned int find_first_set(unsigned long int x) noexcept {
        assert(x != 0);
    #if defined(IGUANA_COMPILER_MSVC)
        unsigned long index = 0;
        _BitScanForward64(&index, x);
        return static_cast<unsigned int>(index);
    #elif defined(IGUANA_COMPILER_GNU) || defined(IGUANA_COMPILER_CLANG)
        return static_cast<unsigned int>(__builtin_ffsl(x));
    #else
        #error Unsupported compiler
    #endif
    }

    inline unsigned int find_first_set(unsigned long long int x) noexcept {
        assert(x != 0);
    #if defined(IGUANA_COMPILER_MSVC)
        unsigned long index = 0;
        _BitScanForward64(&index, static_cast<unsigned __int64>(x));
        return static_cast<unsigned int>(index);
    #elif defined(IGUANA_COMPILER_GNU) || defined(IGUANA_COMPILER_CLANG)
        return static_cast<unsigned int>(__builtin_ffsll(x));
    #else
        #error Unsupported compiler
    #endif
    }

    //

    inline unsigned int count_leading_zeros(unsigned int x) noexcept {
        assert(x != 0);
    #if defined(IGUANA_COMPILER_MSVC)
        return static_cast<unsigned int>(__lzcnt(x));
    #elif defined(IGUANA_COMPILER_GNU) || defined(IGUANA_COMPILER_CLANG)
        return static_cast<unsigned int>(__builtin_clz(x));
    #else
        #error Unsupported compiler
    #endif
    }

    inline unsigned int count_leading_zeros(unsigned long int x) noexcept {
        assert(x != 0);
    #if defined(IGUANA_COMPILER_MSVC)
        return static_cast<unsigned int>(__lzcnt64(x));
    #elif defined(IGUANA_COMPILER_GNU) || defined(IGUANA_COMPILER_CLANG)
        return static_cast<unsigned int>(__builtin_clzl(x));
    #else
        #error Unsupported compiler
    #endif
    }

    inline unsigned int count_leading_zeros(unsigned long long int x) noexcept {
        assert(x != 0);
    #if defined(IGUANA_COMPILER_MSVC)
        return static_cast<unsigned int>(__lzcnt64(x));
    #elif defined(IGUANA_COMPILER_GNU) || defined(IGUANA_COMPILER_CLANG)
        return static_cast<unsigned int>(__builtin_clzll(x));
    #else
        #error Unsupported compiler
    #endif
    }

    //

    inline unsigned int count_trailing_zeros(unsigned int x) noexcept {
        assert(x != 0);
    #if defined(IGUANA_COMPILER_MSVC)
        return static_cast<unsigned int>(_tzcnt_u32(x));
    #elif defined(IGUANA_COMPILER_GNU) || defined(IGUANA_COMPILER_CLANG)
        return static_cast<unsigned int>(__builtin_ctz(x));
    #else
        #error Unsupported compiler
    #endif
    }

    inline unsigned int count_trailing_zeros(unsigned long int x) noexcept {
        assert(x != 0);
    #if defined(IGUANA_COMPILER_MSVC)
        return static_cast<unsigned int>(_tzcnt_u64(x));
    #elif defined(IGUANA_COMPILER_GNU) || defined(IGUANA_COMPILER_CLANG)
        return static_cast<unsigned int>(__builtin_ctzl(x));
    #else
        #error Unsupported compiler
    #endif
    }

    inline unsigned int count_trailing_zeros(unsigned long long int x) noexcept {
        assert(x != 0);
    #if defined(IGUANA_COMPILER_MSVC)
        return static_cast<unsigned int>(_tzcnt_u64(x));
    #elif defined(IGUANA_COMPILER_GNU) || defined(IGUANA_COMPILER_CLANG)
        return static_cast<unsigned int>(__builtin_ctzll(x));
    #else
        #error Unsupported compiler
    #endif
    }

    //

    inline unsigned int count_set(unsigned int x) noexcept {
    #if defined(IGUANA_COMPILER_MSVC)
        return static_cast<unsigned int>(__popcnt(x));
    #elif defined(IGUANA_COMPILER_GNU) || defined(IGUANA_COMPILER_CLANG)
        return static_cast<unsigned int>(__builtin_popcount(x));
    #else
        #error Unsupported compiler
    #endif
    }

    inline unsigned int count_set(unsigned long int x) noexcept {
    #if defined(IGUANA_COMPILER_MSVC)
        return static_cast<unsigned int>(__popcnt64(x));
    #elif defined(IGUANA_COMPILER_GNU) || defined(IGUANA_COMPILER_CLANG)
        return static_cast<unsigned int>(__builtin_popcountl(x));
    #else
        #error Unsupported compiler
    #endif
    }

    inline unsigned int count_set(unsigned long long int x) noexcept {
    #if defined(IGUANA_COMPILER_MSVC)
        return static_cast<unsigned int>(__popcnt64(x));
    #elif defined(IGUANA_COMPILER_GNU) || defined(IGUANA_COMPILER_CLANG)
        return static_cast<unsigned int>(__builtin_popcountll(x));
    #else
        #error Unsupported compiler
    #endif
    }

    //

    template <
        typename T
    > inline std::enable_if_t<std::is_unsigned_v<T>, unsigned int> find_last_set(T x) noexcept {
        assert(x != 0);
        return (sizeof(T)*8 - 1) - count_leading_zeros(x);
    };

    //

    template <
        typename T
    > inline std::enable_if_t<std::is_unsigned_v<T>, unsigned int> length(T v) noexcept {
        // Returns the minimum number of bits required to represent x; the result is 0 for x == 0.
        if (v != T(0)) [[likely]] {
            return find_last_set(v) + 1;
        }
        return 0;
    }
}
