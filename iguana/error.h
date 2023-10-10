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
#include <exception>

namespace iguana {

    enum class error_code : std::uint32_t {
        ok = 0,
        corrupted_bitstream,
        wrong_source_size,
        out_of_input_data,
        insufficient_target_capacity,
        unrecognized_command
    };

    //

    const char* get_error_description(error_code ec);

    //

    class iguana_public exception : public std::exception {
        using super = std::exception;

    protected:
        exception() {}
       
    public:
        virtual ~exception();

    public:
        [[noreturn]] static void from_error(error_code ec);  
    };

    //

    class iguana_public corrupted_bitstream_exception : public exception {
        using super = exception;

    public:
        using super::super;
        virtual ~corrupted_bitstream_exception();    
    };

    //

    class iguana_public wrong_source_size_exception : public exception {
        using super = exception;

    public:
        using super::super;
        virtual ~wrong_source_size_exception();    
    };

    //

    class iguana_public out_of_input_data_exception : public exception {
        using super = exception;

    public:
        using super::super;
        virtual ~out_of_input_data_exception();    
    };

    //

    class iguana_public insufficient_target_capacity_exception : public exception {
        using super = exception;

    public:
        using super::super;
        virtual ~insufficient_target_capacity_exception();    
    };

    //

    class iguana_public unrecognized_command_exception : public exception {
        using super = exception;

    public:
        using super::super;
        virtual ~unrecognized_command_exception();    
    };
}
