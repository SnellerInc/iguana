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
#include "error.h"
#include "ans_encoder.h"
#include "ans_decoder.h"

namespace iguana::ans1 {

    class iguana_public encoder final : public ans::encoder {
        using super = ans::encoder;
        
    private:
        static error_code (*g_Compress)(output_stream& dst, const ans::statistics& stats, const std::uint8_t *src, std::size_t src_len);

    public:
        encoder() noexcept = default;
        virtual ~encoder() noexcept;

        encoder(const encoder&) = delete;
        encoder& operator =(const encoder&) = delete;

        encoder(encoder&& v) = default;
        encoder& operator =(encoder&& v) = default;
        
    public:
        virtual void encode(output_stream& dst, const ans::statistics& stats, const std::uint8_t *src, std::size_t src_len) override final;
        using super::encode;

    private:
        static error_code compress_portable(output_stream& dst, const ans::statistics& stats, const std::uint8_t *src, std::size_t src_len);
    };

    //

    class iguana_public decoder final : public ans::decoder {
        using super = ans::decoder;

    private:
        static error_code (*g_Decompress)(output_stream& dst, std::size_t result_size, input_stream& src, const ans::statistics::decoding_table& tab);

    public: 
        decoder();
        virtual ~decoder() noexcept;

        decoder(const decoder&) = delete;
        decoder& operator =(const decoder&) = delete;

        decoder(decoder&& v) = default;
        decoder& operator =(decoder&& v) = default;

    public:
        virtual void decode(output_stream& dst, std::size_t result_size, input_stream& src, const ans::statistics::decoding_table& tab) override final; 
        using super::decode;       

    private:
        static error_code decompress_portable(output_stream& dst, std::size_t result_size, input_stream& src, const ans::statistics::decoding_table& tab);
    };
}
