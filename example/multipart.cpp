#include <vspp/multipart/basic_parser.hpp>
#include <vspp/multipart/parse.hpp>
#include <vspp/basic_reader.hpp>
#include <vspp/basic_handler.hpp>
#include <iostream>
#include <map>

struct handler_type_1 {
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

using container_type = std::multimap<std::string, std::string>;
using handler_type_2 = vspp::container_handler<container_type>;

int main(int argc, char* argv[]) {
    container_type m;
    // 1
    vspp::multipart::basic_parser<handler_type_1> p1(handler_type_1(), "boundary");
    p1.parse({"--boundary\r\n"
        "Content-Disposition: form-data; name=\"field1\"\r\n\r\n"
        "value1\r\n"
        "--boundary\r\n"
        "Content-Disposition: form-data; name=\"field2\"; filename=\"example.txt\"\r\n\r\n"
        "value2\r\n"
        "--boundary--"});
    // 2
    vspp::multipart::parse(vspp::basic_file_reader<>{"./example/multipart.txt"}, handler_type_2(m), "123456");
    for (auto i=m.begin(); i!=m.end(); ++i) {
        std::cout << "[" << i->first << "] => [" << i->second << "]\n";
    }
    return 0;
}
