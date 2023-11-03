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

#include <stdexcept>
#include "file.h"

iguana::file::file(const std::string& name, const std::string& mode)
  : file() {
    open(name, mode);    
}

iguana::file& iguana::file::operator =(file&& v) {
    if (&v != this) [[likely]] {
        reset(std::exchange(v.m_file, nullptr), std::exchange(v.m_close, false));
    }
    return *this;
}

void iguana::file::close() noexcept {
    if (m_close && (m_file != nullptr)) {
        std::fclose(m_file);
        m_file = nullptr;
        m_close = false;
    }
}

void iguana::file::reset(std::FILE* p, bool should_close) noexcept {
    close();
    m_file = p;
    m_close = should_close;
}

void iguana::file::open(const std::string& name, const std::string& mode) {
    std::FILE* p = nullptr; 
    if (fopen_s(&p, name.c_str(), mode.c_str()) != 0) [[unlikely]] {
        throw std::runtime_error(std::string("cannot open file"));
    }
    reset(p, true);
}

std::uint64_t iguana::file::size() const {
    if (m_file == nullptr) [[unlikely]] {
        throw std::runtime_error(std::string("not associated with any file"));
    }
    std::fpos_t cur_pos = 0;
    if (std::fgetpos(m_file, &cur_pos) != 0) [[unlikely]] {
        throw std::runtime_error(std::string("std::fgetpos() failed"));
    }
    if (std::fseek(m_file, 0, SEEK_END) != 0) [[unlikely]] {
        throw std::runtime_error(std::string("std::fseek() failed"));
    }
    std::fpos_t end_pos = 0;
    if (std::fgetpos(m_file, &end_pos) != 0) [[unlikely]] {
        throw std::runtime_error(std::string("std::fgetpos() failed"));
    }
    if (std::fsetpos(m_file, &cur_pos) != 0) [[unlikely]] {
        throw std::runtime_error(std::string("std::fsetpos() failed"));
    }
    return static_cast<std::uint64_t>(end_pos);    
}

void iguana::file::read(void* p, std::uint64_t n) {
    std::fread(p, 1, static_cast<std::size_t>(n), m_file);
    if (std::ferror(m_file)) [[unlikely]] {
        throw std::runtime_error(std::string("std::fread() failed"));
    }
}
