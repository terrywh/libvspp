#ifndef PARSER_MULTIPART_H
#define PARSER_MULTIPART_H

#include "multipart_core.h"
#include "multipart_parser.h"
#include <string_view>
#include <vector>

namespace parser {
    // 储存器（接口）
    class basic_multipart_parser {
    public:
        basic_multipart_parser() {
            multipart_parser_init(&parser_);
            parser_.data = this;
        }
        virtual ~basic_multipart_parser() = default;
        virtual void on_field(std::string_view field) {}
        virtual void on_field_emit() {}
        virtual void on_value(std::string_view value) {}
        virtual void on_value_emit() {}
        virtual void on_data(std::string_view data) {}
        virtual void on_data_emit() {}
        // 解析文本块
        inline multipart_parser_error_t parse(std::string_view sv) {
            return (multipart_parser_error_t) multipart_parser_execute(&parser_, sv.data(), sv.data() + sv.size());
        }
    protected:
        multipart_parser_t parser_;
    };

    // 执行对应解析器，并存储解析结果
    template <class StringViewReader> // TODO StringViewReader Requirements
    std::size_t read(StringViewReader& sr, basic_multipart_parser& kv) {
        std::size_t read = 0;
        std::string_view sv;
        while( !sr.eof() ) {
            sv = sr.read();
            if(sv.size() > 0 && kv.parse(sv) != MULTIPART_PARSER_ERROR_SUCCESS) break;
            read += sv.size();
        }
        return read;
    }
}

#endif // PARSER_MULTIPART_H
