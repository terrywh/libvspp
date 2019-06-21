#ifndef INCLUDE_TOML_PARSER_HPP
#define INCLUDE_TOML_PARSER_HPP

#include <exception>
#include <functional>
#include <string_view>
#include "parser.h"
#include "constant.h"

extern "C" {

int toml__internal_on_section(
    toml__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int toml__internal_on_field(
    toml__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int toml__internal_on_value(
    toml__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int toml__internal_on_after_value(
    toml__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int toml__internal_on_entry_push(
    toml__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int toml__internal_on_entry_pop(
    toml__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

}

namespace llparse::toml {
    class parser;
    struct parser_setting_s {
        std::function<void (const parser& p, std::string_view chunk)> on_value;
        std::function<void (const parser& p)> on_after_value; // 数据完结
    };
    typedef struct parser_setting_s parser_setting_t;
    
    class parser {
    public:
        parser(const parser_setting_t& setting) noexcept;
        void parse(std::string_view str);
        void reset();
        std::string_view prefix() const noexcept;
        std::string_view field() const noexcept;
        std::uint8_t     container_type() const noexcept;
        std::uint8_t     value_type() const noexcept;
    private:
        toml__internal_t parser_;
        parser_setting_t setting_;
        
        std::string prefix_ = "$";
        std::string field_;
        std::vector<std::uint8_t> container_;
        // 前缀
        friend int ::toml__internal_on_section(toml__internal_t* s, const unsigned char* p, const unsigned char* endp);
        // 字段
        friend int ::toml__internal_on_field(toml__internal_t* s, const unsigned char* p, const unsigned char* endp);
        // friend int ::toml__internal_on_after_field(toml__internal_t* s, const unsigned char* p, const unsigned char* endp);
        // 数据
        friend int ::toml__internal_on_value(toml__internal_t* s, const unsigned char* p, const unsigned char* endp);
        friend int ::toml__internal_on_after_value(toml__internal_t* s, const unsigned char* p, const unsigned char* endp);
        // 层级
        friend int ::toml__internal_on_entry_push(toml__internal_t* s, const unsigned char* p, const unsigned char* endp);
        friend int ::toml__internal_on_entry_pop(toml__internal_t* s, const unsigned char* p, const unsigned char* endp);
        
    };

    class unknown_error: public std::exception {
    public:
        unknown_error(const char* where)
        : where_(where) {
        }
        const char* what() const noexcept;
    private:
        std::string where_;
    };

    class unexpected_token: public std::exception {
    public:
        unexpected_token(char token, const char* where)
        : token_(token)
        , where_(where) {

        }
        const char* what() const noexcept;
    private:
        std::string where_;
        char token_;
    };
}

#endif  /* INCLUDE_TOML_PARSER_HPP */
