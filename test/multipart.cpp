#include "parse_file.hpp"
#include "../src/multipart_parser.hpp"
#include "../src/reader.hpp"

int main(int argc, char* argv[]) {
    parser::multipart_parser parser("====================");

    if(argc < 2) {
        std::cout << "error: test source file required." << std::endl;
        std::exit(-1);
    }
    parser::file_reader fr(argv[1]);
    parser::read(fr, parser);

    for(auto&& entry : parser.data()) {
        for(auto && [field, value] : entry.fields) {
            std::cout << "(" << field << ") => (" << value << ")\n";
        }
        std::cout << "[" << entry.data << "]\n\n";
    }
    return 0;
}
