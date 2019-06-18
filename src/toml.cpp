#include "toml.hpp"
#include <iostream>
#include <string_view>

extern "C" {

int toml__on_field(toml_parser_t* s, const unsigned char* p, const unsigned char* endp) {
    // if(endp > p)
    std::cout << "field => [" << std::string_view { (const char*) p, endp - p } << "]\n";
    return 0;
}

int toml__on_value(toml_parser_t* s, const unsigned char* p, const unsigned char* endp) {
    // if(endp > p)
    std::cout << "value => [" << std::string_view { (const char*) p, endp - p } << "]\n";
    return 0;
}

int toml__on_finish(toml_parser_t* s) {
    std::cout << "finish\n";
    return 0;
}

} // extern "C"

