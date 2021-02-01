#ifndef VSP_BASIC_FILE_READER_H
#define VSP_BASIC_FILE_READER_H

#include <cassert>
#include <cstdint>
#include <algorithm>
#include <string_view>
#include <fstream>

namespace vsp {
    //
    template <std::size_t BufferSize = 4096>
    class basic_file_reader {
    public:
        basic_file_reader(const char* filepath)
        : fs_(filepath) {
            assert(fs_.is_open());
        }

        std::string_view read() {
            fs_.read(buffer_, sizeof(buffer_));
            return std::string_view(buffer_, fs_.gcount());
        }

        bool eof() {
            return fs_.eof();
        }
    private:
        std::ifstream fs_;
        char buffer_[BufferSize];
    };
}

#endif // VSP_BASIC_FILE_READER_H
