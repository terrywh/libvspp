#ifndef VSPP_MULTIPART_PARSE_H
#define VSPP_MULTIPART_PARSE_H

#include "basic_parser.hpp"

namespace vspp { namespace multipart {
    // 
    template <class Buffer, class Handler, typename = decltype(std::declval<Buffer>().data())>
    void parse(const Buffer& buffer, Handler&& handler, std::string_view boundary) {
        basic_parser<Handler> parser { std::forward<Handler>(handler), boundary };
        parser.parse({buffer.data(), buffer.size()});
    }
    // 
    template <class Reader, class Handler, typename = decltype(std::declval<Reader>().eof())>
    void parse(Reader&& reader, Handler&& handler, std::string_view boundary) {
        basic_parser<Handler> parser { std::forward<Handler>(handler), boundary };
        while(!reader.eof()) {
            parser.parse(reader.read());
        }
    }
}}

#endif // VSP_MULTIPART_PARSE_H