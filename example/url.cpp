#include "../src/url/basic_parser.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    vsp::url::basic_parser bp {"http://user:pass@www.abc.com:12345/path?query"};
    std::cout << bp.scheme() << std::endl;
    std::cout << bp.user() << std::endl;
    std::cout << bp.path() << std::endl;
    std::cout << bp.host() << std::endl;
    std::cout << bp.port() << std::endl;
    std::cout << bp.path() << std::endl;
    std::cout << bp.query() << std::endl;
    return 0;
}
