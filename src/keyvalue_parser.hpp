#ifndef PARSER_KEYVALUE_STORE_H
#define PARSER_KEYVALUE_STORE_H

#include "keyvalue.hpp"
#include <map>
#include <memory>
#include <string>
#include <string_view>

namespace parser {
    // 解析器
    class keyvalue_parser: public basic_keyvalue_parser {
    public:
        // 构建 KEY/VALUE 键值对解析器（使用指定的分隔符配置），并将解析结果存入指定容器
        keyvalue_parser(std::map<std::string, std::string>& ctr, const char seps[7]);
        // 构建 KEY/VALUE 键值对解析器（使用指定的分隔符配置），并将解析结果写入内部容器
        keyvalue_parser( const char seps[7]);
        //
        ~keyvalue_parser();
        virtual void on_field(std::string_view field) override;
        // virtual void on_field_emit() override;
        virtual void on_value(std::string_view value) override;
        virtual void on_value_emit() override; // 存在可能 value 为空？
        // 取出当前解析结果
        std::map<std::string, std::string> data() {
            return std::move(*ctr_);
        }
        // TODO Asio Handler
    private:
        char seps_[7]; // 分隔符
        std::pair<std::string, std::string> buffer_;
        std::map<std::string, std::string>*    ctr_;
        bool                                   own_; // 标识容器属主（是否需要释放）
    };
}

#endif // PARSER_KEYVALUE_STORE_H