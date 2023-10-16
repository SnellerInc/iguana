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
#include "error.h"
#include "input_stream.h"
#include "output_stream.h"

namespace iguana {
    class IGUANA_API decoder {
        friend internal::initializer<decoder>;

    private:
        class substream;
        struct context;

    private:
        static void (*g_Decompress)(context& ctx);
        static const internal::initializer<decoder> g_Initializer;

    public:
        decoder() {}
        ~decoder() noexcept;

        decoder(const decoder&) = delete;
        decoder& operator =(const decoder&) = delete;

        decoder(decoder&&) = default;
        decoder& operator =(decoder&&) = default;

    private:
        static void decompress_portable(context& ctx);
//void iguana::decoder::decode(output_stream& dst, input_stream& src, std::uint64_t uncompressed_len, std::int64_t ctrl_cursor) {
        static std::uint64_t read_control_var_uint(const std::uint8_t* src, std::size_t& cursor);
        static void wild_copy(output_stream& dst, std::size_t offs, std::size_t len);
        static void at_process_start();
        static void at_process_end();
    };

    //

    class decoder::substream final {
    public:
        enum : unsigned {
            tokens = 0,
	        offset16,
	        offset24,
	        var_lit_len,
	        var_match_len,
	        literals,
            //
	        count
        };

    public:
        substream() {}
        ~substream() noexcept {}

    public:
        bool empty() const noexcept;
        std::size_t remaining() const noexcept;
        std::uint8_t fetch8(error_code& ec) noexcept;
        std::uint16_t fetch16(error_code& ec) noexcept;
        std::uint32_t fetch24(error_code& ec) noexcept;
        std::uint32_t fetch_var_uint(error_code& ec) noexcept;
        const_byte_span fetch_sequence(std::size_t n, error_code& ec) noexcept;
    };

    //

    struct decoder::context final {
        substream       streams[substream::count];
        output_stream&  dst;
        std::int64_t    last_offset;
        error_code      ec;
    };
}
