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
#include <vector>
#include "common.h"

//

namespace iguana::ans {

    class bitstream final {
  	    std::uint64_t               m_acc = 0;
	    int                         m_cnt = 0;
        std::vector<std::uint8_t>   m_buf;

    public:
        bitstream() = default;
        ~bitstream() noexcept = default;

        bitstream(const bitstream&) = delete;
        bitstream& operator =(const bitstream&) = delete;

        bitstream(bitstream&&) = default;
        bitstream& operator =(bitstream&&) = default;

    public:
        void append(std::uint32_t v, std::uint32_t k);
        void flush();

        auto size() const noexcept {
            return m_buf.size();
        }

        auto data() const noexcept {
            return m_buf.data();
        }
    };
}
