#ifndef VSPP_BASIC_FILE_READER_H
#define VSPP_BASIC_FILE_READER_H

#include <fstream>
#include <string_view>
#include <cassert>

namespace vspp {
// buffer - concept
template <class T>
struct is_buffer {
    template <typename U> static auto test(int) -> typename std::enable_if<
            std::is_invocable_r<const char*, decltype(&U::data), U&>::value && // u.data() -> const char*
            std::is_invocable_r<std::size_t, decltype(&U::size), U&>::value, // u.size() -> std::size_t
            std::true_type
        >::type;
    template <typename> static auto test(...) -> std::false_type;
    static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
};
// reader - concept
template <class T>
struct is_reader {
    template <typename U> static auto test(int) -> typename std::enable_if<
            std::is_invocable_r<std::string_view, decltype(&U::read), U&>::value && // u.read() -> std::string_view
            std::is_invocable_r<bool, decltype(&U::eof), U&>::value, // u.bool() -> bool
            std::true_type
        >::type;
    template <typename> static auto test(...) -> std::false_type;
    static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
};
// reader - 文件
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
static_assert(is_reader<basic_file_reader<>>::value);
}

#endif // VSP_BASIC_FILE_READER_H
