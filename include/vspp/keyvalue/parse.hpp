#ifndef VSPP_KEYVALUE_PARSE_H
#define VSPP_KEYVALUE_PARSE_H

#include "delimiter.hpp"
#include "basic_parser.hpp"
#include "../basic_reader.hpp"

namespace vspp { namespace keyvalue {
    // 
    template <class Buffer, class Handler, typename = typename std::enable_if<is_buffer<Buffer>::value>::type>
    void parse(const Buffer& buffer, Handler&& handler, const delimiter& delim = "\0\0=\0\0&") {
        basic_parser<Handler> parser { std::forward<Handler>(handler), delim };
        parser.parse({buffer.data(), buffer.size()});
        parser.end();
    }
    // 
    template <class Reader, class Handler, typename = typename std::enable_if<is_reader<Reader>::value>::type>
    void parse(Reader&& reader, Handler&& handler, const delimiter& delim = "\0\0=\0\0&") {
        basic_parser<Handler> parser { std::forward<Handler>(handler), delim };
        while(!reader.eof()) {
            parser.parse(reader.read());
        }
        parser.end();
    }
}}

#endif // VSP_KEYVALUE_PARSE_H