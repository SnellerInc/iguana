
#pragma once
#include <cstdint>

namespace iguana {
    enum class error_code : uint32_t {
        ok = 0,
        corrupted_bitstream,
        wrong_source_size,
        out_of_input_data,
        insufficient_target_capacity,
        unrecognized_command
    };

    const char* get_error_description(error_code ec);
}
