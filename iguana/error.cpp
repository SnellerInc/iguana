
#include "error.h"

const char* iguana::get_error_description(error_code ec) {
    switch(ec) {
    case error_code::ok:
        return "success";

    case error_code::corrupted_bitstream:
        return "bitstream corruption detected";

    case error_code::wrong_source_size:
        return "wrong source size";

	case error_code::out_of_input_data:
        return "out of input bytes";

	case error_code::unrecognized_command:
        return "unrecognized command";

	case error_code::insufficient_target_capacity:
        return "insufficient target capacity";
    };
}
