#include "parse_file.hpp"
#include <cstring>
#include <keyvalue.h>

extern "C" {
    int keyvalue_parser_field(
        keyvalue_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        
        std::cout << "(field: " << std::string_view(reinterpret_cast<const char*>(p), endp - p) << ") ";
        return 0;
    }

    int keyvalue_parser_value(
        keyvalue_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {

        std::cout << "(value: " << std::string_view(reinterpret_cast<const char*>(p), endp - p) << ") ";
        return 0;
    }
}

template <class T>
void null_deleter(T* t) {}

int main(int argc, char* argv[]) {
    keyvalue_parser_t s;
    keyvalue_parser_init(&s);
    memcpy(s.seperator, "[]={};", sizeof(s.seperator));

    if(argc < 2) {
        std::cout << "error: test source file required." << std::endl;
        std::exit(-1);
    }
    parse_file p {argv[1]};
    p.run(keyvalue_parser_execute, &s);
    return 0;
}
