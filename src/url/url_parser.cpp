#include "url_parser.hpp"

namespace parser {

    std::regex url_parser::r_ { R"regex(^([^:]+)://(([^:@]+)(:([^@]+))?@)?([^:/]+)(:([^/]+))?(/([^\?]+))?(\?(.+))?$)regex" };

    url_parser::url_parser(std::string_view url) {
        if(!std::regex_match(url.begin(), url.end(), m_, r_)) {
            throw std::runtime_error("failed to parse url: ill-formed");
        }
    }
    
}