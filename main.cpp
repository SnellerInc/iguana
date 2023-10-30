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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <stdexcept>
#include <iostream>
#include <variant>
#include <fstream>
#include <map>
#include "iguana/error.h"

// TODO: use a proper makefile
#include "iguana/common.cpp"
#include "iguana/ans_byte_statistics.cpp"
#include "iguana/ans_nibble_statistics.cpp"
#include "iguana/ans1.cpp"
#include "iguana/ans32.cpp"
#include "iguana/ans_nibble.cpp"
#include "iguana/ans_bitstream.cpp"
#include "iguana/error.cpp"
#include "iguana/entropy.cpp"
#include "iguana/output_stream.cpp"
#include "iguana/decoder.cpp"
#include "iguana/encoder.cpp"
#include "iguana/c_bindings.cpp"

//

namespace iguana {
    class command_line final {
        class parser;
        using parameter_type = std::variant<std::string, double, std::monostate>;
        using map_type = std::map<std::string, parameter_type>;
        
    private:
        const map_type m_options;
         
    public:
        command_line(int argc, const char* const* const argv);
        ~command_line() {}

        command_line(const command_line&) = delete;
        command_line& operator =(const command_line&) = delete;

    public:
        bool contains(const std::string& name) const noexcept;

        template <
            typename T
        > T get(const std::string& name) const;

        template <
            typename T
        > T get(const std::string& name, T default_value) const;
    };

    //

    template <
        typename T
    > T command_line::get(const std::string& name) const {
        const auto r = m_options.find(name);
        if (r == m_options.cend()) {
           throw std::invalid_argument(std::string("the option '") + name + "' has not been provided");
        }
        return std::get<T>(r->second);
    }

    template <
        typename T
    > T command_line::get(const std::string& name, T default_value) const {
        if (!contains(name)) {
            return default_value;
        }
        return this->template get<T>(name);
    }

    //

    class command_line::parser final {
        const int                m_argc = 0;
        const char* const* const m_argv = nullptr;
        int                      m_cursor = 0;
        map_type                 m_options;

    public:
        parser(int argc, const char* const* const argv)
          : m_argc(argc)
          , m_argv(argv) {}

        ~parser() {}

        parser(const parser&) = delete;
        parser& operator =(const parser&) = delete;

        parser(parser&&) = delete;
        parser& operator =(parser&&) = delete;
  
    public:
        map_type parse();
    
    private:
        std::string get_string_parameter_for(const char* name);
        double get_double_parameter_for(const char* name);

        template <
            typename T
        > void add(const char* short_name, const char* long_name, T value);

        void add(const char* short_name, const char* long_name);
    };

    //

    template <
        typename T
    > void command_line::parser::add(const char* short_name, const char* long_name, T value) {
        if (m_options.contains(short_name)) {
            throw std::invalid_argument(std::string("the option '") + short_name + "' has already been specified");
        }

        if (m_options.contains(long_name)) {
            throw std::invalid_argument(std::string("the option '") + long_name + "' has already been specified");
        }

        m_options[short_name] = value;
        m_options[long_name] = std::move(value);
    }
}

//

iguana::command_line::command_line(int argc, const char* const* const argv)
  : m_options(parser(argc, argv).parse()) {}

bool iguana::command_line::contains(const std::string& name) const noexcept {
    return m_options.contains(name);
}

iguana::command_line::map_type iguana::command_line::parser::parse() {
    for(m_cursor = 1; m_cursor < m_argc;) {
        const char* const opt = m_argv[m_cursor++];

        if ((std::strcmp(opt, "-h") == 0) || (std::strcmp(opt, "--help") == 0)) {
            add("h", "help");
            continue;
        }

        if ((std::strcmp(opt, "-d") == 0) || (std::strcmp(opt, "--decompress") == 0)) {
            add("d", "decompress", get_string_parameter_for(opt));            
            continue;
        }

        if ((std::strcmp(opt, "-o") == 0) || (std::strcmp(opt, "--output") == 0)) {
            add("o", "output", get_string_parameter_for(opt));            
            continue;
        }

        if ((std::strcmp(opt, "-t") == 0) || (std::strcmp(opt, "--threshold") == 0)) {
            const auto v = get_double_parameter_for(opt);
            if ((v < 0.0) || (v > 1.0)) {
                throw std::invalid_argument(std::string("the value for the option '") + opt + "' must be in the range [0.0, 1.0]");
            }
            add("t", "threshold", v);
            continue;
        }

        if ((std::strcmp(opt, "-e") == 0) || (std::strcmp(opt, "--entropy") == 0)) {
            const auto v = get_string_parameter_for(opt);
            if ((v != "none") && (v != "ans32") && (v != "ans") && (v != "ans_nibble")) {
                throw std::invalid_argument(std::string("unrecognized entropy mode '") + v + "' supplied for the option '" + opt + "'");
            }
            add("e", "entropy", v);  
            continue;
        }

        if ((std::strcmp(opt, "-x") == 0) || (std::strcmp(opt, "--encoding") == 0)) {
            const auto v = get_string_parameter_for(opt);
            if ((v != "raw") && (v != "iguana")) {
                throw std::invalid_argument(std::string("unrecognized encoding '") + v + "' supplied for the option '" + opt + "'");
            }
            add("x", "encoding", v);  
            continue;
        }

        throw std::invalid_argument(std::string("unrecognized option '") + opt + "'");
    }

    return std::move(m_options);
}

std::string iguana::command_line::parser::get_string_parameter_for(const char* opt) {
    if (m_cursor >= m_argc) {
        throw std::invalid_argument(std::string("no argument provided for '") + opt + "'");
    }

    const char* const param = m_argv[m_cursor];

    if (param == nullptr) {
        throw std::invalid_argument(std::string("null argument provided for '") + opt + "'");
    }

    if (std::strlen(param) == 0) {
        throw std::invalid_argument(std::string("empty argument provided for '") + opt + "'");
    }

    if (param[0] == '-') {
        throw std::invalid_argument(std::string("no argument provided for '") + opt + "'");
    }

    ++m_cursor;
    return param;
}

double iguana::command_line::parser::get_double_parameter_for(const char* opt) {
    return std::stod(get_string_parameter_for(opt));
}

void iguana::command_line::parser::add(const char* short_name, const char* long_name) {
    add(short_name, long_name, std::monostate{});
}

//

int main(int argc, char *argv[]) try {

    iguana::command_line options(argc, argv);
    if (options.contains("help")) {
        std::cout << argv[0] << " [args] [-o file]" << std::endl;
        std::cout << "  -h, --help" << std::endl;
        std::cout << "  -o, --output file" << std::endl;
        std::cout << "  -d, --decompress" << std::endl;
        std::cout << "  -t, --threshold" << std::endl;
        std::cout << "  -e, --entropy" << std::endl;
        std::cout << "  -x, --encoding" << std::endl;
        return EXIT_SUCCESS;
    }

    {   std::ofstream str_out;
        std::ostream* p_str_out = &std::cout;

        if (options.contains("output")) {
            const auto path = options.get<std::string>("output"); 
            str_out.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
            p_str_out = &str_out;
        }

        if (options.contains("decompress")) {
        
        } else {

            iguana::output_stream dst;

            {   const iguana::encoder::part ep = {
                    .m_entropy_mode = iguana::entropy_mode_from_string(options.get<std::string>("entropy", "ans32").c_str()),
                    .m_encoding = iguana::encoding_from_string(options.get<std::string>("encoding", "iguana").c_str()),
                    .m_rejection_threshold = options.get<double>("threshold", 1.0)
                };

                iguana::encoder enc;
                enc.encode(dst, ep);
            }

            printf("dst len = %zu\n", dst.size());
        }
    }
    return EXIT_SUCCESS;

} catch(const std::exception& e) {
    std::cerr << "exception: " << e.what() << std::endl;
    return EXIT_FAILURE;

} catch(...) {
    std::cerr << "unrecognized exception" << std::endl;
    return EXIT_FAILURE;
}
