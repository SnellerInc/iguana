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
#include <memory>
#include <vector>
#include "common.h"
#include "span.h"
#include "error.h"
#include "entropy.h"
#include "output_stream.h"
#include "command.h"

//

namespace iguana {
    enum class encoding : std::uint8_t {
        raw = 0,        // No structural compression is applied
        iguana = 1 << 0 // Iguana structural compression is applied
    };

    encoding encoding_from_string(const char* name);
    const char* to_string(encoding e);

    //
    
    class IGUANA_API encoder {
        friend internal::initializer<encoder>;

    public:

        struct part final {
            const std::uint8_t* m_data;
            std::size_t         m_size;
            entropy_mode        m_entropy_mode;
            encoding            m_encoding;
            double              m_rejection_threshold;
        };

    public:
        static constexpr inline double default_rejection_threshold = 1.0;

    private:
        static const internal::initializer<encoder> g_Initializer;
     
    private:
        std::vector<std::uint8_t>   m_control;
        std::ptrdiff_t              m_last_command_offset = -1;
        output_stream               m_entropy_data;

    public:
        encoder();
        ~encoder();

        encoder(const encoder&) = delete;
        encoder& operator =(const encoder&) = delete;

        encoder(encoder&&) = default;
        encoder& operator =(encoder&&) = default;
     
    public:
        void encode(output_stream& dst, const part& p);
        void encode(output_stream& dst, const part* first, const part* last);

        void encode(output_stream& dst, const part* first, std::size_t n_parts) {
            encode(dst, first, first + n_parts);
        }

        void encode(output_stream& dst, const std::uint8_t* p, std::size_t n);

    private:
        static void at_process_start();
        static void at_process_end();

        //
        
        void encode_part(output_stream& dst, const part& p);
        void encode_iguana(output_stream& dst, const part& p);
        void encode_entropy_raw(output_stream& dst, const part& p);

        template <
            typename T_ENCODER
        > void encode_entropy(output_stream& dst, const part& p);

        //

        void append_control_var_uint(std::uint64_t v);
        void append_control_command(command cmd);
    };
}
