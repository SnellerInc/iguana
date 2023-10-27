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

//

#include <cstdint>
#include <cassert>
#include <type_traits>
#include "platform.h"

//

namespace iguana {
    using ssize_t = std::make_signed_t<std::size_t>;

    namespace internal {
        template <
            typename T
        > class initializer final {
        public:
            initializer() noexcept {
                T::at_process_start();
            }

            ~initializer() noexcept {
                T::at_process_end();
            }

            initializer(const initializer&) = delete;
            initializer& operator =(const initializer&) = delete;

            initializer(initializer&&) = delete;
            initializer& operator =(initializer&&) = delete;
        };
    }
}

#define IGUANA_UNIMPLEMENTED for(;;) { std::abort(); } // For porting purposes only, remove me when done!
