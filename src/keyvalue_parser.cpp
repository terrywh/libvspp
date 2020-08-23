#include "keyvalue_parser.hpp"
#include <cstring> // std::memcpy

namespace parser {
    // 构建 KEY/VALUE 键值对解析器（使用指定的分隔符配置），并将解析结果存入指定容器
    keyvalue_parser::keyvalue_parser(std::map<std::string, std::string>& ctr, const char seps[7])
    : ctr_(&ctr), own_(false) {
        std::memcpy(seps_, seps, sizeof(seps_));
        keyvalue_parser_init(&parser_);
        parser_.seperator = seps_;
        parser_.data = this;
    }
    // 构建 KEY/VALUE 键值对解析器（使用指定的分隔符配置），并将解析结果写入内部容器
    keyvalue_parser::keyvalue_parser(const char seps[7])
    : ctr_(new std::map<std::string, std::string>()), own_(true) {
        std::memcpy(seps_, seps, sizeof(seps_));
        parser_.seperator = seps_;
    }
    keyvalue_parser::~keyvalue_parser() {
        if(own_) delete ctr_;
    }
    //
    void keyvalue_parser::on_field(std::string_view field) {
        buffer_.first.append(field);
    }
    //
    void keyvalue_parser::on_value(std::string_view value) {
        buffer_.second.append(value);
    }
    //
    void keyvalue_parser::on_value_emit() {
        ctr_->insert( std::move(buffer_) );
    }
}