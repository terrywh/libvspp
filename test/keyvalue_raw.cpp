#include "parse_file.hpp"
#include <cstring>
#include <keyvalue_parser.h>

extern "C" {
    int keyvalue_parser_on_field(
        keyvalue_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        
        std::cout << "(field: " << std::string_view((const char*)p, endp-p) << ") ";
        return 0;
    }

    int keyvalue_parser_on_field_emit(
        keyvalue_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        
        std::cout << "(/field) ";
        return 0;
    }

    int keyvalue_parser_on_value(
        keyvalue_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {

        std::cout << "(value: " << std::string_view((const char*)p, endp-p) << ") ";
        return 0;
    }

    int keyvalue_parser_on_value_emit(
        keyvalue_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        
        std::cout << "(/value) ";
        return 0;
    }
}

int main(int argc, char* argv[]) {
    keyvalue_parser_t s;
    keyvalue_parser_init(&s);
    s.seperator = (char*)"[]={};";

    if(argc < 2) {
        std::cout << "error: test source file required." << std::endl;
        std::exit(-1);
    }
    parse_file p {argv[1]};
    p.run(keyvalue_parser_execute, &s);
    return 0;
}
