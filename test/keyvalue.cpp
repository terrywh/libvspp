#include "parse_file.hpp"
#include <cstring>
#include <keyvalue.hpp>
#include "../src/reader.hpp"

int main(int argc, char* argv[]) {
    std::map<std::string, std::string> rs;
    parser::keyvalue kv(rs, {"[]={};"});

    if(argc < 2) {
        std::cout << "error: test source file required." << std::endl;
        std::exit(-1);
    }
    parser::file_reader fr(argv[1]);
    parser::read(fr, kv);

    for(auto&& [key, val] : rs) {
        std::cout << "(" << key << ") => (" << val << ")\n";
    }
    return 0;
}
