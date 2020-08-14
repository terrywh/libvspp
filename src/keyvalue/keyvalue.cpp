#include "keyvalue.hpp"
#include "../reader.hpp"

extern "C" {
    int keyvalue_parser_on_field(
        keyvalue_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        auto self = reinterpret_cast<parser::keyvalue*>(s->data);
        self->get()->on_field(std::string_view(reinterpret_cast<const char*>(p), endp - p));
        return 0;
    }

    int keyvalue_parser_on_field_emit(
        keyvalue_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        auto self = reinterpret_cast<parser::keyvalue*>(s->data);
        self->get()->on_field_emit();
        return 0;
    }

    int keyvalue_parser_on_value(
        keyvalue_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        auto self = reinterpret_cast<parser::keyvalue*>(s->data);
        self->get()->on_value(std::string_view(reinterpret_cast<const char*>(p), endp - p));
        return 0;
    }

    int keyvalue_parser_on_value_emit(
        keyvalue_parser_t* s, const unsigned char* p,
        const unsigned char* endp) {
        auto self = reinterpret_cast<parser::keyvalue*>(s->data);
        self->get()->on_value_emit();
        return 0;
    }
}

namespace parser {
    //
    void keyvalue_store::on_field(std::string_view field) {
        entry_.first.append(field);
    }
    //
    void keyvalue_store::on_value(std::string_view value) {
        entry_.second.append(value);
    }
    //
    void keyvalue_store::on_value_emit() {
        ctr_->insert( std::move(entry_) );
    }
}