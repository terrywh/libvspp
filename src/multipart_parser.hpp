#ifndef PARSER_MULTIPART_PARSER_H
#define PARSER_MULTIPART_PARSER_H

#include "multipart.hpp"
#include <map>
#include <string>
#include <vector>

namespace parser {
    //
    struct multipart_entry {
        using fields_type = std::map<std::string, std::string>;
        using entry_type = std::pair<std::string, std::string>;
        // 数据前置 Header 字段
        fields_type  fields;
        // 数据
        std::string    data;
    };
    // 解析器
    class multipart_parser: public basic_multipart_parser {
    public:
        using value_type = multipart_entry;
        // 直接指定 boundary 设置（若 boundary 存在包裹引号(")会自动忽略）
        // 注意：不包含额外的前缀、后缀的 "--" 符号
        multipart_parser(std::string_view boundary);
        //
        virtual void on_field(std::string_view field) override {
            field_.first.append(field);
        }
        virtual void on_value(std::string_view value) override {
            field_.second.append(value);
        }
        virtual void on_value_emit() override {
            entry_.fields.insert(std::move(field_));
        }
        virtual void on_data(std::string_view data) override {
            entry_.data.append(data);
        }
        virtual void on_data_emit() override {
            ctr_.push_back(std::move(entry_));
        }
        // 获取当前已解析的部分数据
        inline std::vector<value_type> data() {
            return move(ctr_);
        }
    private:
        std::vector<value_type>           ctr_;
        typename value_type::entry_type field_;
        value_type                      entry_;
    };
}

#endif // PARSER_MULTIPART_PARSER_H
