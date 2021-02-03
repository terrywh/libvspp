#ifndef VSPP_KEYVALUE_BASIC_PARSER_H
#define VSPP_KEYVALUE_BASIC_PARSER_H

#include "delimiter.hpp"
#include <string_view>
#include <cassert>
#include <cstdlib> // for abort
#include <cctype>  // for isspace

namespace vspp { namespace keyvalue {
    
    // 用于 KEY/VALUE 键值对解析器（使用指定的分隔符）
    // 注意：
    //  1. Handler 须满足 copyable / movable
    //  2. 渐进式解析器，暂不支持无包裹符号时的中部留白；
    template <class Handler> 
    class basic_parser {
    public:
        typedef enum {
            BEFORE_FIELD,
                   FIELD_START,
                   FIELD,
             AFTER_FIELD,
                OPERATOR,
            BEFORE_VALUE,
                   VALUE_START,
                   VALUE,
             AFTER_VALUE,
               SEPERATOR,
        } state_t;
        using delimiter_type = delimiter;
        using handler_type = typename std::decay<Handler>::type;
        using field_type = typename handler_type::field_type;
        using value_type = typename handler_type::value_type;
        // 
        basic_parser(handler_type&& handler, delimiter_type delim)
        : handler_( std::forward<handler_type>(handler) )
        , delimiter_(delim)
        , state_(BEFORE_FIELD)
        , span_(-1) {
            
        }
        //
        ~basic_parser() {}

#include "../span_start.hpp"

        // 解析指定区段
        bool parse(std::string_view data) {
            char c;
            for(int i=0;i<data.size();++i) {
                c = data[i];
                switch(state_) {
                case BEFORE_FIELD:
                    if(c == delimiter_.before_field) {
                        state_ = FIELD_START;
                        break;
                    }
                    if(std::isspace(c)) break;
                    [[fallthrough]];
                case FIELD_START:
                    SPAN_START(FIELD);
                    [[fallthrough]];
                case FIELD:
                    if('\0' == delimiter_.after_field) {
                        if(std::isspace(c)) {
                            SPAN_END(AFTER_FIELD, on_field);
                            break;
                        }
                        else if(c == delimiter_.operater) {
                            SPAN_END(BEFORE_VALUE, on_field);
                            break;
                        }
                    }
                    else {
                        if(c == delimiter_.after_field) {
                            SPAN_END(AFTER_FIELD, on_field);
                            break;
                        }
                    }
                    break;
                case AFTER_FIELD:
                    if(c == delimiter_.operater) {
                        state_ = BEFORE_VALUE;
                        break;
                    }
                    else if(std::isspace(c)) break;
                    return false;
                case OPERATOR:
                    std::abort();
                    break;
                case BEFORE_VALUE:
                    if(std::isspace(c)) break;
                    else if(c == delimiter_.before_value) {
                        state_ = VALUE_START;
                        break;
                    }
                    else if(c == delimiter_.seperator) {
                        on_entry();
                        state_ = BEFORE_FIELD;
                        break;
                    }
                    [[fallthrough]];
                case VALUE_START:
                    SPAN_START(VALUE);
                    [[fallthrough]];
                case VALUE:
                    if('\0' == delimiter_.after_value) {
                        if(std::isspace(c)) {
                            SPAN_END(AFTER_VALUE, on_value);
                            on_entry();
                            break;
                        }
                        else if(c == delimiter_.seperator) {
                            SPAN_END(BEFORE_FIELD, on_value);
                            on_entry();
                            break;
                        }
                    }
                    else {
                        if(c == delimiter_.after_value) {
                            SPAN_END(AFTER_VALUE, on_value);
                            on_entry();
                            break;
                        }
                    }
                    break;
                case AFTER_VALUE:
                    if(c == delimiter_.seperator) {
                        state_ = BEFORE_FIELD;
                        break;
                    }
                    else if(std::isspace(c)) break;
                    return false;
                case SEPERATOR:
                    std::abort();
                    break;
                }
            }
            if(span_ != -1) {
                if(state_ == FIELD) on_field( data.substr(span_) );
                else if(state_ == VALUE) on_value( data.substr(span_) );
                span_ = -1;
            }
            return true;
        }
        // 
        void end() {
            if(!entry_.first.empty()) {
                on_entry(); // 可能 value 为空，但还需上报整个 entry
            }
        }

#include "../span_end.hpp"

    private:
        handler_type     handler_;
        delimiter_type delimiter_; // 分隔符
        int                state_; // 解析状态
        std::pair<field_type, value_type> entry_;
        int span_; // 区段起始位置

        inline void on_field(std::string_view field) {
            entry_.first.append(field);
        }
        inline void on_value(std::string_view value) {
            entry_.second.append(value);
        }
        inline void on_entry() {
            handler_.on_entry(std::move(entry_)); 
            assert(entry_.first.empty()); // 用户应 “拿走” 数据
        }
    }; 
} }


#endif // VSP_KEYVALUE_BASIC_PARSER_H