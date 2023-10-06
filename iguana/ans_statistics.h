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
#include "ans_histogram.h"

namespace iguana::ans {

    class iguana_public raw_statistics {
        histogram h; // TODO

    public:
        raw_statistics() = default;
        ~raw_statistics() noexcept = default;

        raw_statistics(const raw_statistics&) noexcept = default;
        raw_statistics& operator =(const raw_statistics&) noexcept = default;

        raw_statistics(raw_statistics&&) noexcept = default;
        raw_statistics& operator =(raw_statistics&&) noexcept = default;

    public:
        void compute(std::uint8_t *p, std::size_t n) noexcept;

        void compute(byte_span s) noexcept {
            compute(s.data(), s.size());
        }
    };

    class statistics {
          
    };
}
