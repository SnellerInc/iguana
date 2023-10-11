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
#include "input_stream.h"
#include "output_stream.h"

namespace iguana {
    class iguana_public decoder {
        friend internal::initializer<decoder>;

    private:
        struct context final {
            
        };

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
        static void at_process_start();
        static void at_process_end();
    };
}
