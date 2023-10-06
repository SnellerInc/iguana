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
#include <array>
#include "common.h"
#include "memops.h"
#include "span.h"

namespace iguana::ans {

    class iguana_public histogram {
        std::array<std::size_t, 256>     freqs;
        std::array<std::size_t, 256 + 1> cumulative_freqs;

    public:
        histogram() {
            memory::zero(freqs);
            memory::zero(cumulative_freqs);
        }

        ~histogram() noexcept = default;

        histogram(const histogram&) noexcept = default;
        histogram& operator =(const histogram&) noexcept = default;

        histogram(histogram&&) noexcept = default;
        histogram& operator =(histogram&&) noexcept = default;

    public:
        int compute(std::uint8_t *p, std::size_t n) noexcept;

        int compute(byte_span s) noexcept {
            return compute(s.data(), s.size());
        }
    };
}
