
#include <unistd.h>
#include <iostream>
#include <string>
#include "../src/keyvalue/basic_parser.hpp"
#include "../src/keyvalue/parse.hpp"
#include "../src/basic_file_reader.hpp"

struct handler {
    using field_type = std::string;
    using value_type = std::string;
    void on_entry(std::pair<field_type, value_type>&& entry) {
        std::pair<field_type, value_type> e (std::move(entry));
        std::cout << "[" << e.first << "] => [" << e.second << "]\n";
    }
};

int main(int argc, char* argv[]) {
    // 1
    vspp::keyvalue::basic_parser<handler> p1 { handler(), "[]={};"};
    p1.parse("[aaa] = {bbb}; [a b ] = { 123 }");
    p1.end();
    // 2
    vspp::keyvalue::basic_parser<handler> p2 { handler(), "\0\0:\0\r\n"};
    p2.parse("Field: Value\r\nField:  Key=\"value\";  Key=\"value\"; \r\n");
    // p2.end();
    // 3
    vspp::keyvalue::parse(vspp::basic_file_reader<>{"./example/keyvalue.txt"}, handler(), "[]={};");
    // 4
    vspp::keyvalue::parse(std::string_view{"'aaa':'bbb'; 'a b': '1 2 3';"}, handler(), "'':'';");
    return 0;
}
