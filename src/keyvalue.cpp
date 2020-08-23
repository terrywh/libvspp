#include "keyvalue.hpp"

extern "C" {
    int keyvalue_parser_on_field(
        keyvalue_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        auto self = reinterpret_cast<parser::basic_keyvalue_parser*>(s->data);
        self->on_field(std::string_view(reinterpret_cast<const char*>(p), endp - p));
        return 0;
    }

    int keyvalue_parser_on_field_emit(
        keyvalue_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        auto self = reinterpret_cast<parser::basic_keyvalue_parser*>(s->data);
        self->on_field_emit();
        return 0;
    }

    int keyvalue_parser_on_value(
        keyvalue_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        auto self = reinterpret_cast<parser::basic_keyvalue_parser*>(s->data);
        self->on_value(std::string_view(reinterpret_cast<const char*>(p), endp - p));
        return 0;
    }

    int keyvalue_parser_on_value_emit(
        keyvalue_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        auto self = reinterpret_cast<parser::basic_keyvalue_parser*>(s->data);
        self->on_value_emit();
        return 0;
    }
}
