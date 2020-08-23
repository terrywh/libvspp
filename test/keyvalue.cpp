#include "parse_file.hpp"
#include "../src/keyvalue_parser.hpp"
#include "../src/reader.hpp"

int main(int argc, char* argv[]) {
    parser::keyvalue_parser kv({"[]={};"});

    if(argc < 2) {
        std::cout << "error: test source file required." << std::endl;
        std::exit(-1);
    }
    parser::file_reader fr(argv[1]);
    parser::read(fr, kv);

    for(auto&& [key, val] : kv.data()) {
        std::cout << "(" << key << ") => (" << val << ")\n";
    }
    return 0;
}
