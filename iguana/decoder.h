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
    
        //

        class entropy_buffer final {
        public:
            static constexpr const std::size_t default_size = 1 << 20;

        private:
            std::uint8_t*   m_data = nullptr;
            std::size_t     m_cursor = 0;
            std::size_t     m_capacity = 0;

        public:
            explicit entropy_buffer(std::size_t n = default_size);
            ~entropy_buffer();

            entropy_buffer(const entropy_buffer&) = delete; 
            entropy_buffer& operator =(const entropy_buffer&) = delete; 
            entropy_buffer(entropy_buffer&& v);
            entropy_buffer& operator =(entropy_buffer&& v);

        public:
            std::size_t cursor() const noexcept {
                return m_cursor;
            }

            std::size_t capacity() const noexcept {
                return m_capacity;
            }

            void reset() noexcept {
                m_cursor = 0;
            }

            void reset(std::size_t n);

        private:
            static std::pair<std::uint8_t*, std::size_t> acquire_memory(std::size_t n);
            static void release_memory(std::uint8_t* p);
        };

    private:
        static void (*g_Decompress)(context& ctx);
        static const internal::initializer<decoder> g_Initializer;

    private:
        entropy_buffer m_ent_buf;

    public:
        decoder() {}
        ~decoder() noexcept;

        decoder(const decoder&) = delete;
        decoder& operator =(const decoder&) = delete;

        decoder(decoder&&) = default;
        decoder& operator =(decoder&&) = default;

    public:
        void decode(output_stream& dst, input_stream& src);

    private:
        void decompress(output_stream& dst, const std::uint8_t* const src, std::uint64_t uncompressed_len, ssize_t& ctrl_cursor);
        static void decompress_portable(context& ctx);
        static std::uint64_t read_control_var_uint(const std::uint8_t* src, ssize_t& cursor);
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

    private:
        const std::uint8_t* m_cursor = nullptr;
        const std::uint8_t* m_end = nullptr;

    public:
        substream() noexcept {}

        substream(const std::uint8_t* p, const std::uint8_t* e) noexcept
          : m_cursor(p)
          , m_end(e) {}

        substream(const std::uint8_t* p, std::size_t n) noexcept
          : substream(p, p + n) {}

        ~substream() noexcept {}

        substream(const substream&) = default;
        substream& operator =(const substream&) = default;
        substream(substream&&) = default;
        substream& operator =(substream&&) = default;

    public:

        bool empty() const noexcept {
            return m_cursor == m_end;
        }

        std::size_t remaining() const noexcept {
            return std::size_t(m_end - m_cursor);
        }

        void set(const std::uint8_t* p, const std::uint8_t* e) noexcept {
            m_cursor = p;
            m_end = e;
        }

        void set(const std::uint8_t* p, std::size_t n) noexcept {
            set(p, p + n);
        }

        std::uint8_t fetch8(error_code& ec) noexcept; 
        std::uint8_t fetch8(); 
        std::uint16_t fetch16(error_code& ec) noexcept;
        std::uint16_t fetch16();
        std::uint32_t fetch24(error_code& ec) noexcept;
        std::uint32_t fetch24();
        std::uint32_t fetch_var_uint(error_code& ec) noexcept;
        std::uint32_t fetch_var_uint();
        const_byte_span fetch_sequence(std::size_t n, error_code& ec) noexcept;
        const_byte_span fetch_sequence(std::size_t n);
    };

    //

    struct decoder::context final {
        substream       streams[substream::count];
        output_stream&  dst;
        std::int64_t    last_offset;
        error_code      ec;
    };
}
