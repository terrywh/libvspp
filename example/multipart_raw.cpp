#include "parse_file.hpp"
#include <multipart_parser.h>

extern "C" {
    int multipart_parser_on_field(
        multipart_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        
        std::cout << "(field: " << std::string_view(reinterpret_cast<const char*>(p), endp - p) << ") ";
        return 0;
    }

    int multipart_parser_on_field_emit(
        multipart_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        
        std::cout << "(/field) ";
        return 0;
    }

    int multipart_parser_on_value(
        multipart_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {

        
        std::cout << "(value: " << std::string_view(reinterpret_cast<const char*>(p), endp - p) << ") ";
        return 0;
    }

    int multipart_parser_on_value_emit(
        multipart_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        
        std::cout << "(/value) ";
        return 0;
    }

    int multipart_parser_on_data(
        multipart_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {

        std::cout << "(data: " << std::string_view(reinterpret_cast<const char*>(p), endp - p) << ") ";
        return 0;
    }

    int multipart_parser_on_data_emit(
        multipart_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        
        std::cout << "(/data) ";
        return 0;
    }

    int multipart_parser_on_complete(
        multipart_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {

        std::cout << "(complete) ";
        return 0;
    }
}

int main(int argc, char* argv[]) {
    multipart_parser_t s;
    multipart_parser_init(&s);
    s.boundary = const_cast<char*>("====================");
    
    if(argc < 2) {
        std::cout << "error: test source file required." << std::endl;
        std::exit(-1);
    }
    parse_file p {argv[1]};
    p.run(multipart_parser_execute, &s);
    return 0;
}
