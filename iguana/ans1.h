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
#include "ans_encoder.h"
#include "ans_decoder.h"

namespace iguana::ans1 {

    class iguana_public encoder final : public ans::encoder {
        using super = ans::encoder;

    private:
	    std::uint32_t m_state = ans::word_L;
        
    public:
        encoder();
        virtual ~encoder() noexcept;

        encoder(const encoder&) = delete;
        encoder& operator =(const encoder&) = delete;

        encoder(encoder&& v) = default;
        encoder& operator =(encoder&& v) = default;
        
    public:
        virtual error_code encode(const std::uint8_t *src, std::size_t n, const ans::statistics& stats) override final;
        using super::encode;

        virtual void clear() override final;

    private:
        void compress_portable(const std::uint8_t *src, std::size_t n, const ans::statistics& stats);

        void compress(const std::uint8_t *src, std::size_t n, const ans::statistics& stats) {
            compress_portable(src, n, stats);
        }

        void put(std::uint8_t v, const ans::statistics& stats);
        void flush();
    };

    //

    class iguana_public decoder final : public ans::decoder {
        using super = ans::decoder;

    private:
	    std::uint32_t m_state = ans::word_L;

    public: 
        decoder();
        virtual ~decoder() noexcept;

        decoder(const decoder&) = delete;
        decoder& operator =(const decoder&) = delete;

        decoder(decoder&& v) = default;
        decoder& operator =(decoder&& v) = default;           
    };
}
