
#include <unistd.h>
#include <iostream>
#include <string>
#include <vspp/keyvalue/basic_parser.hpp>
#include <vspp/keyvalue/parse.hpp>
#include <vspp/basic_reader.hpp>
#include <vspp/basic_handler.hpp>
#include <map>

struct handler_type_1 {
    using field_type = std::string;
    using value_type = std::string;
    void on_entry(std::pair<field_type, value_type>&& entry) {
        std::pair<field_type, value_type> e (std::move(entry));
        std::cout << "[" << e.first << "] => [" << e.second << "]\n";
    }
};

using container_type = std::multimap<std::string, std::string>;
using handler_type_2 = vspp::container_handler<container_type>;

int main(int argc, char* argv[]) {
    container_type m;
    // 1
    vspp::keyvalue::basic_parser<handler_type_1> p1 { handler_type_1(), "[]={};"};
    p1.parse("[aaa] = {bbb}; [a b ] = { 123 }");
    p1.end();
    // 2
    vspp::keyvalue::basic_parser<handler_type_2> p2 { handler_type_2(m), "\0\0:\0\r\n"};
    p2.parse("Field: Value\r\nField:  K1=\"v1\"; K2=\"v2\";\r\n");
    for (auto i=m.begin(); i!=m.end(); ++i) {
        std::cout << "[" << i->first << "] => [" << i->second << "]\n";
    }
    // p2.end();
    // 3
    vspp::keyvalue::parse(vspp::basic_file_reader<>{"./example/keyvalue.txt"}, handler_type_1(), "[]={};");
    // 4
    vspp::keyvalue::parse(std::string_view{"'aaa':'bbb'; 'a b': '1 2 3';"}, handler_type_1(), "'':'';");
    return 0;
}
