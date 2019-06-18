#include "../src/toml.hpp"
#include <cstring>

int main(int argc, char* argv[]) {
    toml__internal_t toml;
    toml__internal_init(&toml);

    for(int i=1;i<argc;++i) {
        toml__internal_execute(&toml, argv[i], argv[i] + std::strlen(argv[i]));
    }
    const char* end = "\0";
    toml__internal_execute(&toml, end, end + 1);
    return 0;
}
