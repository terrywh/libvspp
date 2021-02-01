#ifndef VSP_URL_BASIC_PARSER_H
#define VSP_URL_BASIC_PARSER_H

#include <regex>
#include <string_view>

namespace vspp { namespace url {
    // 用于支持 URL 解析
    class url_parser {
    public:
        // 用于解析如：scheme://user:pass@host:port/path?query 形式的 URL 解析
        url_parser(std::string_view url);

        inline std::string_view scheme() const {
            return gm(m_[1]);
        }
        inline std::string_view user() const {
            return gm(m_[3]);
        }
        inline std::string_view pass() const {
            return gm(m_[5]);
        }
        inline std::string_view host() const {
            return gm(m_[6]);
        }
        inline std::string_view port() const {
            return gm(m_[8]);
        }
        inline std::string_view path() const {
            return gm(m_[10]);
        }
        inline std::string_view query() const {
            return gm(m_[12]);
        }
    private:
        std::cmatch       m_;
        // 读取匹配子组对应字符串视图
        std::string_view gm(const std::csub_match& m) const {
            return {m.first, m.second - m.first};
        }
        static std::regex r_;
    };
}}

#endif // VSP_URL_BASIC_PARSER_H
