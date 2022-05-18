#ifndef VSPP_MULTIPART_BASIC_PARSER_H
#define VSPP_MULTIPART_BASIC_PARSER_H

#include <string_view>
#include <type_traits>
#include <cassert>
#include <cstdlib> // for abort
#include <cctype>  // for isspace

namespace vspp { namespace multipart {
    // 解析器（接口）
    // TODO Handler requirement
    // on_entry(entry_type&& )
    // on_data(data_type&& )
    // field_type.append && movable
    // value_type.append && movable
    // data_type.append && movable
    template <class Handler>
    class basic_parser {
    public:
        typedef enum {
     MAYBE_BEFORE_BOUDNARY, // 
           BEFORE_BOUNDARY, // -
           BEFORE_BOUDNARY_1, // -
                  BOUNDARY, // boundary
            AFTER_BOUNDARY, // -
            AFTER_BOUNDARY_1, // -
            AFTER_BOUNDARY_2, // (ERROR)
                
              BEFORE_FIELD, // \r\n / SP
                     FIELD, // field
               AFTER_FIELD, // :

              BEFORE_VALUE, // SP 
                     VALUE, // value (\r)
               AFTER_VALUE, // (\r) \n

         MAYBE_BEFORE_DATA, // \r
               BEFORE_DATA, // \n
                      DATA,
        } state_t;
        using handler_type = typename std::decay<Handler>::type;
        using entry_type = std::pair<typename handler_type::field_type, typename handler_type::value_type>;
        using cache_type = typename handler_type::cache_type;

        basic_parser(handler_type handler, std::string_view boundary)
        : handler_( handler )
        , boundary_(boundary)
        , state_(BEFORE_BOUNDARY)
        , index_(0)
        , span_(-1) {
            // 可能包裹引号
            if(boundary_.front() == '"' && boundary_.back() == '"')
                boundary_ = boundary_.substr(1, boundary_.size() - 2);
        }
        ~basic_parser() = default;

#include "../span_start.hpp"
        // 解析文本块
        bool parse(std::string_view data) {
            char c;
            for(std::size_t i=0;i<data.size();++i) {
                c = data[i];
                switch(state_) {
                case MAYBE_BEFORE_BOUDNARY:
                    if(c == '\n') {
                        state_ = BEFORE_BOUNDARY;
                        break;
                    }
                    SPAN_START(DATA);
                    on_data({remedy_, 1}); // 补全前置 DATA 数据 (\r)
                    break;
                case BEFORE_BOUNDARY:
                    if(c == '-') {
                        state_ = BEFORE_BOUDNARY_1;
                        break;
                    }
                    SPAN_START(DATA);
                    on_data({remedy_, 2}); // 补全前置 DATA 数据 (\r\n)
                    break;
                case BEFORE_BOUDNARY_1: // -
                    if(c == '-') {
                        state_ = BOUNDARY;
                        break;
                    }
                    SPAN_START(DATA);
                    on_data({remedy_, 3}); // 不全前置 DATA 数据 (\r\n-)
                    break;
                case BOUNDARY:
                    if(boundary_[index_] == c) {
                        ++index_;
                        if(index_ == boundary_.size()) {
                            on_emit();
                            state_ = AFTER_BOUNDARY;
                            index_ = 0;
                        }
                        break;
                    }
                    on_data({remedy_, 4}); // 补全前置 DATA 数据 (\r\n-- + boundary ~ index)
                    if(index_ > 0) {
                        on_data(boundary_.substr(0, index_));
                        index_ = 0;
                    }
                    if(c == '\r')
                        state_ = MAYBE_BEFORE_BOUDNARY;
                    else
                        SPAN_START(DATA);
                    break;
                case AFTER_BOUNDARY:
                    if(c == '\r') {
                        state_ = BEFORE_FIELD;
                        break;
                    }
                    else if(c == '-') {
                        state_ = AFTER_BOUNDARY_1;
                        break;
                    }
                    return false;
                case AFTER_BOUNDARY_1:
                    if(c == '-') {
                        state_ = AFTER_BOUNDARY_2;
                        break;
                    }
                case AFTER_BOUNDARY_2:
                    return false;
                
                case BEFORE_VALUE:
                    if(std::isspace(c)) break;
                    SPAN_START(VALUE);
                    [[fallthrough]];
                case VALUE:
                    if(c == '\r') {
                        SPAN_END(AFTER_VALUE, on_value);
                        on_entry();
                        break;
                    }
                    break;
                case AFTER_VALUE: // \n
                    if(c == '\n') break;

                    state_ = MAYBE_BEFORE_DATA;
                    [[fallthrough]];
                case MAYBE_BEFORE_DATA:
                    if(c == '\r') {
                        state_ = BEFORE_DATA;
                        break;
                    }

                    state_ = BEFORE_FIELD;
                    [[fallthrough]];
                case BEFORE_FIELD:
                    if(std::isspace(c)) break;
                    
                    SPAN_START(FIELD);
                    [[fallthrough]];
                case FIELD:
                    if(c == ':') {
                        SPAN_END(BEFORE_VALUE, on_field);
                        break;
                    }
                    break;
                case AFTER_FIELD:
                    std::abort();
                    break;
                case BEFORE_DATA: // \n
                    if(c == '\n') break;
                    
                    SPAN_START(DATA);
                    [[fallthrough]];
                case DATA:
                    if(c == '\r') {
                        SPAN_END(MAYBE_BEFORE_BOUDNARY, on_data);
                        break;
                    }
                    break;
                }
            }
            if(span_ != -1) {
                if(state_ == FIELD) on_field(data.substr(span_));
                else if(state_ == VALUE) on_value(data.substr(span_));
                else on_data(data.substr(span_));
                span_ = -1;
            }
            return true;
        }

#include "../span_end.hpp"
        
    private:
        void on_field(std::string_view field) {
            entry_.first.append(field.data(), field.size());
        }
        void on_value(std::string_view value) {
            entry_.second.append(value.data(), value.size());
        }
        void on_entry() {
            handler_.on_entry(std::move(entry_));
        }
        void on_data(std::string_view data) {
            cache_.append(data);
        }
        void on_emit() {
            handler_.on_data(std::move(cache_));
        }
        
        handler_type      handler_;
        std::string_view boundary_;
        entry_type          entry_;
        cache_type          cache_;
        int                 state_;
        int                 index_;
        int                  span_;

        static const char remedy_ []; // 用于在匹配了 boundary 前缀，但后续并未完全匹配时进行弥补
    };

    template <class Handler>
    const char basic_parser<Handler>::remedy_[] = {'\r','\n','-','-'};
}}

#endif // VSP_MULTIPART_BASIC_PARSER_H
