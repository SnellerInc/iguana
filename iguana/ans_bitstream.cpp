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

#include "ans_bitstream.h"

//

void iguana::ans::bitstream::append(std::uint32_t v, std::uint32_t k) {
	const std::uint32_t m = ~(~std::uint32_t(0) << k);
	m_acc |= std::uint64_t(v & m) << m_cnt;
	m_cnt += int(k);

	while(m_cnt >= 8) {
		m_buf.push_back(std::uint8_t(m_acc));
		m_acc >>= 8;
		m_cnt -= 8;
	}
}

void iguana::ans::bitstream::flush() {
	while(m_cnt > 0) {
		m_buf.push_back(std::uint8_t(m_acc));
		m_acc >>= 8;
		m_cnt -= 8;
	}
}
