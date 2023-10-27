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

//

namespace iguana {
    class IGUANA_API encoder {
        friend internal::initializer<encoder>;

    private:
        static const internal::initializer<encoder> g_Initializer;

    public:
        encoder();
        ~encoder();

        encoder(const encoder&) = delete;
        encoder& operator =(const encoder&) = delete;

        encoder(encoder&&) = default;
        encoder& operator =(encoder&&) = default;
     
    public:
        void encode(const std::uint8_t* p, std::size_t n);

    private:
        static void at_process_start();
        static void at_process_end();
    };
}
