#include "multipart.hpp"
#include <iostream>
#include <string_view>

extern "C" {
    int multipart_parser_on_field(
        multipart_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        
        auto self = reinterpret_cast<parser::basic_multipart_parser*>(s->data);
        self->on_field(std::string_view(reinterpret_cast<const char*>(p), endp - p));
        return 0;
    }

    int multipart_parser_on_field_emit(
        multipart_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        
        auto self = reinterpret_cast<parser::basic_multipart_parser*>(s->data);
        self->on_field_emit();
        return 0;
    }

    int multipart_parser_on_value(
        multipart_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {

        auto self = reinterpret_cast<parser::basic_multipart_parser*>(s->data);
        self->on_value(std::string_view(reinterpret_cast<const char*>(p), endp - p));
        return 0;
    }

    int multipart_parser_on_value_emit(
        multipart_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {

        auto self = reinterpret_cast<parser::basic_multipart_parser*>(s->data);
        self->on_value_emit();
        return 0;
    }

    int multipart_parser_on_data(
        multipart_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        
        auto self = reinterpret_cast<parser::basic_multipart_parser*>(s->data);
        self->on_data(std::string_view(reinterpret_cast<const char*>(p), endp - p));
        return 0;
    }

    int multipart_parser_on_data_emit(
        multipart_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {

        auto self = reinterpret_cast<parser::basic_multipart_parser*>(s->data);
        self->on_data_emit();
        return 0;
    }

    int multipart_parser_on_complete(
        multipart_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {

        // std::cout << "(complete) ";
        return 0;
    }
}