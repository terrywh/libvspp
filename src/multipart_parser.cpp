#include "multipart_parser.hpp"
#include <stdexcept>

namespace parser {
    // 直接指定 boundary 设置（若 boundary 存在包裹引号(")会自动忽略）
    // 注意：不包含额外的前缀、后缀的 "--" 符号
    multipart_parser::multipart_parser(std::string_view boundary) {
        if(boundary.front() == '"' && boundary.back() == '"') {
            parser_.boundary = const_cast<char*>(boundary.data() + 1);
            parser_.boundary_length = boundary.size() - 2;
        }
        else {
            parser_.boundary = const_cast<char*>(boundary.data());
            parser_.boundary_length = boundary.size();
        }
    }
}