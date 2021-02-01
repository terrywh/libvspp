#ifndef VSP_KEYVALUE_PARSE_H
#define VSP_KEYVALUE_PARSE_H

#include "delimiter.hpp"
#include "basic_parser.hpp"

namespace vsp { namespace keyvalue {
    // 
    template <class Buffer, class Handler, typename = decltype(std::declval<Buffer>().data())>
    void parse(const Buffer& buffer, Handler&& handler, const delimiter& delim = "\0\0=\0\0&") {
        basic_parser<Handler> parser { std::forward<Handler>(handler), delim };
        parser.parse({buffer.data(), buffer.size()});
        parser.end();
    }
    // 
    template <class Reader, class Handler, typename = decltype(std::declval<Reader>().eof())>
    void parse(Reader&& reader, Handler&& handler, const delimiter& delim = "\0\0=\0\0&") {
        basic_parser<Handler> parser { std::forward<Handler>(handler), delim };
        while(!reader.eof()) {
            parser.parse(reader.read());
        }
        parser.end();
    }
}}

#endif // VSP_KEYVALUE_PARSE_H