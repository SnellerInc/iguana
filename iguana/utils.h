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
}
