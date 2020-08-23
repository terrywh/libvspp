#ifndef PARSER_KEYVALUE_H
#define PARSER_KEYVALUE_H

#include "keyvalue_parser.h"
#include <string_view>

namespace parser {
    // 基本解析器
    class basic_keyvalue_parser {
    public:
        basic_keyvalue_parser() {
            keyvalue_parser_init(&parser_);
            parser_.data = this;
        }
        virtual ~basic_keyvalue_parser() = default;
        virtual void on_field(std::string_view field) {}
        virtual void on_field_emit() {} //
        virtual void on_value(std::string_view value) {}
        virtual void on_value_emit() {} // 存在可能 value 为空？
        // 解析文本块
        inline keyvalue_error_t parse(std::string_view sv) {
            return (keyvalue_error_t) keyvalue_parser_execute(&parser_, sv.data(), sv.data() + sv.size());
        }
    protected:
        keyvalue_parser_t parser_;
    };
    
    // 执行对应解析器，并存储解析结果
    template <class StringViewReader> // TODO StringViewReader Requirements
    std::size_t read(StringViewReader& sr, basic_keyvalue_parser& kv) {
        std::size_t read = 0;
        std::string_view sv;
        while( !sr.eof() ) {
            sv = sr.read();
            if(sv.size() > 0 && kv.parse(sv) != KEYVALUE_PARSER_ERROR_SUCCESS) break;
            read += sv.size();
        }
        return read;
    }
    // TODO Boost.Asio 对应 Read / AsyncRead 实现
}

#endif // PARSER_KEYVALUE_PARSER_CORE_H
