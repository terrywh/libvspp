#include "parse_file.hpp"
#include <toml.h>

extern "C" {
    int toml_parser_on_field(
        toml_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {

        std::cout << "(field: " << std::string_view(reinterpret_cast<const char*>(p), endp - p) << ") " << std::flush;
        return 0;
    }

    int toml_parser_on_field_emit(
        toml_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {

        std::cout << "(/field) " << std::flush;
        return 0;
    }

    int toml_parser_on_value(
        toml_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {

        std::cout << "(value: " << std::string_view(reinterpret_cast<const char*>(p), endp - p) << ") " << std::flush;
        return 0;
    }

    int toml_parser_on_value_emit(
        toml_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {

        std::cout << "(/value) " << std::flush;
        return 0;
    }

}

int main(int argc, char* argv[]) {
    toml_parser_t s;
    toml_parser_init(&s);
    
    if(argc < 2) {
        std::cout << "error: test source file required." << std::endl;
        std::exit(-1);
    }
    parse_file p {argv[1]};
    p.run(toml_parser_execute, &s);
    return 0;
}
