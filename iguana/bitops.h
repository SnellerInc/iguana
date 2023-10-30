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

namespace iguana::bit {

    inline unsigned int find_first_set(unsigned int x) {
        return static_cast<unsigned int>(__builtin_ffs(x));
    }

    inline unsigned int find_first_set(unsigned long int x) {
        return static_cast<unsigned int>(__builtin_ffsl(x));
    }

    inline unsigned int find_first_set(unsigned long long int x) {
        return static_cast<unsigned int>(__builtin_ffsll(x));
    }

    //

    inline unsigned int count_leading_zeros(unsigned int x) {
        return static_cast<unsigned int>(__builtin_clz(x));
    }

    inline unsigned int count_leading_zeros(unsigned long int x) {
        return static_cast<unsigned int>(__builtin_clzl(x));
    }

    inline unsigned int count_leading_zeros(unsigned long long int x) {
        return static_cast<unsigned int>(__builtin_clzll(x));
    }

    //

    inline unsigned int count_trailing_zeros(unsigned int x) {
        return static_cast<unsigned int>(__builtin_ctz(x));
    }

    inline unsigned int count_trailing_zeros(unsigned long int x) {
        return static_cast<unsigned int>(__builtin_ctzl(x));
    }

    inline unsigned int count_trailing_zeros(unsigned long long int x) {
        return static_cast<unsigned int>(__builtin_ctzll(x));
    }

    //

    inline unsigned int count_set(unsigned int x) {
        return static_cast<unsigned int>(__builtin_popcount(x));
    }

    inline unsigned int count_set(unsigned long int x) {
        return static_cast<unsigned int>(__builtin_popcountl(x));
    }

    inline unsigned int count_set(unsigned long long int x) {
        return static_cast<unsigned int>(__builtin_popcountll(x));
    }
}
