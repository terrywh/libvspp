#include "toml.hpp"
#include <cstdio>
#ifndef NDEBUG
    #include <iostream>
#endif
#include <string_view>
#include "error.h"

extern "C" {
    // 前缀
    // ---------------------------------------------------------------------------------------------------------------------

    int toml__internal_on_section(toml__internal_t* s, const unsigned char* p, const unsigned char* endp) {
        auto * self = static_cast<llparse::toml::parser*>(s->data);
        self->prefix_.resize(1);
#ifndef NDEBUG
        std::cout << "> on_prefix_start\n";
#endif
        return 0;
    }
    // 字段
    // ---------------------------------------------------------------------------------------------------------------------
    int toml__internal_on_field(toml__internal_t* s, const unsigned char* p, const unsigned char* endp) {
        auto * self = static_cast<llparse::toml::parser*>(s->data);
        if(endp > p) {
            self->field_.append((const char*)p, static_cast<std::size_t>(endp - p));
#ifndef NDEBUG
            std::cout << "> on_field => [" << std::string_view { (const char*) p, static_cast<std::size_t>(endp - p) } << "]\n";
#endif
        }
        return 0;
    }
    // 数据
    // ---------------------------------------------------------------------------------------------------------------------

    int toml__internal_on_value(toml__internal_t* s, const unsigned char* p, const unsigned char* endp) {
        const auto * self = static_cast<llparse::toml::parser*>(s->data);
        if(endp > p) {
            if(self->setting_.on_value) self->setting_.on_value(*self, {(const char*) p, static_cast<std::size_t>(endp - p)});
#ifndef NDEBUG
            std::cout << "> on_value => [" << std::string_view { (const char*) p, static_cast<std::size_t>(endp - p) } << "]\n";
#endif
        }
        
        return 0;
    }
    int toml__internal_on_after_value(toml__internal_t* s, const unsigned char* p, const unsigned char* endp) {
        auto * self = static_cast<llparse::toml::parser*>(s->data);
        if(self->setting_.on_after_value) self->setting_.on_after_value(*self);
#ifndef NDEBUG
        std::cout << "> on_after_value: " << (int) s->container_type << "/" << (int)s->value_type << "\n";
#endif
        self->field_.clear();
        return 0;
    }
    // 层级关系
    // ---------------------------------------------------------------------------------------------------------------------
    int toml__internal_on_entry_push(toml__internal_t* s, const unsigned char* p, const unsigned char* endp) {
        auto * self = static_cast<llparse::toml::parser*>(s->data);
        // std::cout << "> on_entry1: << " << self->prefix_ << "\n";
        self->prefix_.push_back('.');
        self->prefix_.append(self->field_.empty() ? "#" : self->field_);
        if(s->container_type == (std::uint8_t) llparse::toml::CONTAINER_TYPE_ARRAY_TABLE) {
            self->prefix_.append(".#");
        }
        self->field_.clear();
        if(self->container_.back() == (std::uint8_t) llparse::toml::CONTAINER_TYPE_ARRAY 
            && s->container_type == (std::uint8_t) llparse::toml::CONTAINER_TYPE_TABLE) {
            self->container_.push_back((std::uint8_t) llparse::toml::CONTAINER_TYPE_ARRAY_TABLE);
            s->container_type = (std::uint8_t) llparse::toml::CONTAINER_TYPE_ARRAY_TABLE;
        }else{
            self->container_.push_back(s->container_type);
        }
#ifndef NDEBUG
        std::cout << "> on_entry2: << " << self->prefix_ << "\n";
#endif
        return 0;
    }

    int toml__internal_on_entry_pop(toml__internal_t* s, const unsigned char* p, const unsigned char* endp) {
        auto * self = static_cast<llparse::toml::parser*>(s->data);
        // std::cout << "> on_after_entry1: << " << (int)s->container_type << "/" << (int)s->value_type << " (" << self->prefix_ << ")\n";
        self->prefix_.resize(self->prefix_.find_last_of("."));
        self->container_.pop_back();
        s->container_type = self->container_.back();
#ifndef NDEBUG
        std::cout << "> on_after_entry2: << " << (int)s->container_type << "/" << (int)s->value_type << " (" << self->prefix_ << ")\n";
#endif
        return 0;
    }

} // extern "C"

namespace llparse::toml {

    parser::parser(const parser_setting_t& setting) noexcept
    : setting_(setting) {
        toml__internal_init(&parser_);
        parser_.data = this;
        container_.push_back((std::uint8_t) llparse::toml::CONTAINER_TYPE_TABLE);
    }

    void parser::parse(std::string_view str) {
        switch(toml__internal_execute(&parser_, str.data(), str.data() + str.size())) {
        case ERROR_SUCCESS:
            break;
        case ERROR_UNEXPECTED_TOKEN:
            throw unexpected_token(parser_.error_pos[0], parser_.reason);
        default:
            throw unknown_error(parser_.reason);
        }
    }

    void parser::reset() {
        toml__internal_init(&parser_);
        container_.resize(1);
    }

    std::string_view parser::prefix() const noexcept  {
        return {prefix_.data(), prefix_.size()};
    }

    std::string_view parser::field() const noexcept  {
        return {field_.data(), field_.size()};
    }

    std::uint8_t parser::container_type() const noexcept {
        return parser_.container_type;
    }

    std::uint8_t parser::value_type() const noexcept  {
        return parser_.value_type;
    }


    const char* unknown_error::what() const noexcept {
        static char buffer[64];
        std::sprintf(buffer, "TOML parse failed: unknown error @%s", where_.c_str());
        return buffer;
    }

    const char* unexpected_token::what() const noexcept {
        static char buffer[96]; 
        std::sprintf(buffer, "TOML parse failed: unexpected token (0x%02x => '%c') @%s", token_, token_, where_.c_str());
        return buffer;
    }
}