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

#include "output_stream.h"

void iguana::output_stream::append(const value_type* p, size_type n) {
    // TODO: optimize me
    for(auto* const e = p + n; p != e; ++p) {
        append(*p);
    }
}

void iguana::output_stream::append(const output_stream& s) {
    // TODO: optimize me
    append(s.data(), s.size());
}

void iguana::output_stream::append_reverse(const value_type* p, size_type n) {
    // TODO: optimize me
    for(auto* e = p + n; e != p;) {
        append(*--e);
    }
}

void iguana::output_stream::append_reverse(const output_stream& s) {
    // TODO: optimize me
    append_reverse(s.data(), s.size());
}
