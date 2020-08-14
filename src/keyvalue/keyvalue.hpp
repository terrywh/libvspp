#ifndef PARSER_KEYVALUE_H
#define PARSER_KEYVALUE_H

#include "keyvalue_parser.h"
#include <array>
#include <string_view>
#include <memory>
#include <string>
#include <map>

namespace parser {
    // 储存器（接口）
    class basic_keyvalue_store {
    public:
        virtual ~basic_keyvalue_store() = default;
        virtual void on_field(std::string_view field) {}
        virtual void on_field_emit() {}
        virtual void on_value(std::string_view value) {}
        virtual void on_value_emit() {}
    };
    // 存储器（实现）
    class keyvalue_store: public basic_keyvalue_store {
    public:
        keyvalue_store(std::map<std::string, std::string>& ctr)
        : ctr_(&ctr)
        , own_(false) {}
        keyvalue_store()
        : ctr_(new std::map<std::string, std::string>())
        , own_(true) {}
        ~keyvalue_store() {
            if(own_) delete ctr_;
        }
        void on_field(std::string_view field) override;
        // void on_field_emit() override;
        void on_value(std::string_view value) override;
        void on_value_emit() override;
    private:
        std::pair<std::string, std::string> entry_;
        std::map<std::string, std::string>*   ctr_;
        bool                                  own_; // 标识容器属主（是否需要释放）
    };
    // 解析器
    class keyvalue {
    public:
        struct options_t {
            // 分隔符
            char seperators[7]; // "[]={};"
        };
        // 构建 KEY/VALUE 键值对解析器（使用指定的分隔符配置），并将解析结果存入指定容器
        keyvalue(const std::shared_ptr<basic_keyvalue_store>& ctr, const options_t& opts)
        : store_(ctr)
        , opts_(opts) {
            keyvalue_parser_init(&parser_);
            parser_.seperator = opts_.seperators;
            parser_.data = this;
        }
        // 构建 KEY/VALUE 键值对解析器（使用指定的分隔符配置），并将解析结果存入指定容器
        keyvalue(std::map<std::string, std::string>& ctr, const options_t& opts)
        : store_(new keyvalue_store(ctr))
        , opts_(opts) {
            keyvalue_parser_init(&parser_);
            parser_.seperator = opts_.seperators;
            parser_.data = this;
        }
        // 构建 KEY/VALUE 键值对解析器（使用指定的分隔符配置），并将解析结果写入内部容器
        keyvalue(const options_t& opts)
        : store_(new keyvalue_store())
        , opts_(opts) {
            keyvalue_parser_init(&parser_);
            parser_.seperator = opts_.seperators;
            parser_.data = this;
        }
        // 解析文本块
        inline keyvalue_error_t parse(std::string_view sv) const {
            return (keyvalue_error_t) keyvalue_parser_execute(&parser_, sv.data(), sv.data() + sv.size());
        }
        // TODO Asio Handler

        // 当前存储目标
        inline basic_keyvalue_store* get() {
            return store_.get();
        }
    private:
        std::shared_ptr<basic_keyvalue_store> store_;
        options_t                              opts_;
        mutable keyvalue_parser_t            parser_;
    };
    // 执行对应解析器，并存储解析结果
    template <class StringViewReader> // TODO StringViewReader
    std::size_t read(StringViewReader& sr, keyvalue& kv) {
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