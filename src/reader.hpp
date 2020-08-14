#ifndef PARSER_STRING_SOURCE_H
#define PARSER_STRING_SOURCE_H

#include <cassert>
#include <cstdint>
#include <algorithm>
#include <string_view>
#include <fstream>

namespace parser {

    class memory_string_reader {
    public:
        memory_string_reader(void* memory, std::size_t size)
        : data_((char*)memory)
        , size_(size)
        , from_(0) {}

        template <class Buffer> // TODO Buffer Requirement
        memory_string_reader(const Buffer& buffer)
        : data_(buffer.data())
        , size_(buffer.size())
        , from_(0) {

        }

        std::string_view read() {
            std::size_t size = std::min(size_ - from_, std::size_t(4096));
            std::string_view sv(data_ + from_, size);
            from_ += size;
            return sv;
        }

        bool eof() {
            return from_ == size_;
        }
    private:
        const char* data_;
        std::size_t size_;
        std::size_t from_;
    };

    class file_reader {
    public:
        file_reader(const char* filepath)
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
        char buffer_[4096];
    };
}

#endif // PARSER_STRING_SOURCE_H
