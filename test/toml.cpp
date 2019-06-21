#include "../src/toml/toml.hpp"
#include <cstring>
#include <iostream>

int main(int argc, char* argv[]) {
    std::string value;
    llparse::toml::parser p({
        /*.on_value = */[&value] (const llparse::toml::parser& p, std::string_view chunk) {
            value.append(chunk);
            std::cout << "= on_value: " << (int)p.container_type() << "/" << (int)p.value_type() << " (" << chunk << ")\n";
        },
        /* .on_after_value = */[&value] (const llparse::toml::parser& p) {
            // if(p.prefix() == ".a.b.e") exit(0);
            std::cout << "= on_after_value: " << (int)p.container_type() << "/" << (int)p.value_type() << " (" << p.prefix() << "/" << p.field() << "/" << value << ")\n";
            value.clear();
        },
    });
    std::string flr;
    do {
        std::getline(std::cin, flr);
        flr.append("\n");
        p.parse(flr);
    } while(true);
    return 0;
}
