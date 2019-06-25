#include "toml.hpp"
#include <cstdio>
#include <string_view>
#include "error.h"

extern "C" {
    // 前缀
    // ---------------------------------------------------------------------------------------------------------------------

    int toml__internal_on_section(toml__internal_t* s, const unsigned char* p, const unsigned char* endp) {
        auto * self = static_cast<llparse::toml::parser*>(s->data);
        self->prefix_.resize(1);
        self->index_.resize(1);
        s->array_index = self->index_.back();
        return 0;
    }
    // 字段
    // ---------------------------------------------------------------------------------------------------------------------
    int toml__internal_on_field(toml__internal_t* s, const unsigned char* p, const unsigned char* endp) {
        auto * self = static_cast<llparse::toml::parser*>(s->data);
        if(endp > p) {
            self->field_.append((const char*)p, static_cast<std::size_t>(endp - p));
        }
        return 0;
    }
    // 数据
    // ---------------------------------------------------------------------------------------------------------------------

    int toml__internal_on_value(toml__internal_t* s, const unsigned char* p, const unsigned char* endp) {
        const auto * self = static_cast<llparse::toml::parser*>(s->data);
        if(endp > p) {
            if(self->setting_.on_value) self->setting_.on_value(*self, {(const char*) p, static_cast<std::size_t>(endp - p)});
        }
        
        return 0;
    }
    int toml__internal_on_after_value(toml__internal_t* s, const unsigned char* p, const unsigned char* endp) {
        auto * self = static_cast<llparse::toml::parser*>(s->data);
        if(self->setting_.on_after_value) self->setting_.on_after_value(*self);
        self->field_.clear();
        if(s->container_type == llparse::toml::CONTAINER_TYPE_ARRAY) s->array_index = ++self->index_.back();
        return 0;
    }
    // 层级关系
    // ---------------------------------------------------------------------------------------------------------------------
    int toml__internal_on_entry_push(toml__internal_t* s, const unsigned char* p, const unsigned char* endp) {
        auto * self = static_cast<llparse::toml::parser*>(s->data);
        // std::cout << "> on_entry1: << " << self->prefix_ << "\n";
        if(self->field_.empty()) self->field_.assign("#");
        else if(s->container_type == (std::uint8_t) llparse::toml::CONTAINER_TYPE_ARRAY_TABLE) self->field_.append(".#");
        self->prefix_.push_back(self->field_);
        
        self->field_.clear();
        if(self->container_.back() == (std::uint8_t) llparse::toml::CONTAINER_TYPE_ARRAY) {
            if(s->container_type == (std::uint8_t) llparse::toml::CONTAINER_TYPE_TABLE) {
                s->container_type = (std::uint8_t) llparse::toml::CONTAINER_TYPE_ARRAY_TABLE;
            }
        }
        // 开始一个 ARRAY 节
        if(s->container_type == (std::uint8_t) llparse::toml::CONTAINER_TYPE_ARRAY) {
            self->index_.back() = s->array_index;
            s->array_index = 0;
            self->index_.push_back(s->array_index);
        }
        self->container_.push_back(s->container_type);
        return 0;
    }

    int toml__internal_on_entry_pop(toml__internal_t* s, const unsigned char* p, const unsigned char* endp) {
        auto * self = static_cast<llparse::toml::parser*>(s->data);
        self->prefix_.pop_back();
        std::uint8_t ct = self->container_.back();
        self->container_.pop_back();
        
        s->container_type = self->container_.back();
        // 结束的了一个 array 节
        if(ct == llparse::toml::CONTAINER_TYPE_ARRAY) {
            self->index_.pop_back();
            s->array_index = self->index_.back();
        }
        if(s->container_type == llparse::toml::CONTAINER_TYPE_ARRAY || // 本节结束后上层是一个 array 节
            ct == llparse::toml::CONTAINER_TYPE_ARRAY_TABLE) { // 本节 array_table 结束
            s->array_index = ++self->index_.back();
        }
        return 0;
    }

} // extern "C"

namespace llparse::toml {

    parser::parser(const parser_setting_t& setting) noexcept
    : setting_(setting) {
        toml__internal_init(&parser_);
        parser_.data = this;
        parser_.container_type = llparse::toml::CONTAINER_TYPE_TABLE;
        parser_.array_index = 0;
        prefix_.push_back("$");
        container_.push_back((std::uint8_t) parser_.container_type);
        index_.push_back((std::uint32_t) parser_.array_index);
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

    std::string parser::field() const noexcept  {
        std::string pre = prefix_[0];
        for(int i=1;i<prefix_.size();++i) {
            pre.push_back('.');
            pre.append(prefix_[i]);
        }
        if(field_.empty()) {
            if(parser_.container_type == CONTAINER_TYPE_ARRAY) {
                pre.push_back('.');
                pre.push_back('#');
            }
        }else{
            pre.push_back('.');
            pre.append(field_);
        }
        return pre;
    }

    std::uint8_t parser::value_type() const noexcept  {
        return parser_.value_type;
    }

    std::uint32_t parser::value_array_index() const noexcept  {
        return parser_.array_index;
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