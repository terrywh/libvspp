#include "../src/multipart/basic_parser.hpp"
#include "../src/multipart/parse.hpp"
#include "../src/basic_reader.hpp"
#include <iostream>

struct handler {
    using field_type = std::string;
    using value_type = std::string;
    using cache_type = std::string;
    void on_entry(std::pair<field_type, value_type>&& entry) {
        std::pair<field_type, value_type> e (std::move(entry));
        std::cout << "[" << e.first << "] => [" << e.second << "]\n";
    }
    void on_data(cache_type&& data) {
        std::string s (std::move(data));
        std::cout << "(" << s << ")\n";
    }
};

int main(int argc, char* argv[]) {
    // 1
    vspp::multipart::basic_parser<handler> p1(handler(), "boundary");
    p1.parse({"--boundary\r\n"
        "Content-Disposition: form-data; name=\"field1\"\r\n\r\n"
        "value1\r\n"
        "--boundary\r\n"
        "Content-Disposition: form-data; name=\"field2\"; filename=\"example.txt\"\r\n\r\n"
        "value2\r\n"
        "--boundary--"});
    // 2
    vspp::multipart::parse(vspp::basic_file_reader<>{"./example/multipart.txt"}, handler(), "123456");
    return 0;
}
