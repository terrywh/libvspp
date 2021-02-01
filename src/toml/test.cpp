#include <tree_sitter/api.h>
#include <tree_sitter/parser.h>
#include <cstring>
#include <iostream>
#include <memory>

extern "C" const TSLanguage *tree_sitter_toml(void);

int main() {
    // Create a parser.
    TSParser *parser = ts_parser_new();
    // Set the parser's language (JSON in this case).
    ts_parser_set_language(parser, tree_sitter_toml());

    const char *source_code = R"TOML(
[abcd]
a = 1
b = "2"
c = 3.3
d = 2021-01-28 17:53
)TOML";

    TSTree *tree = ts_parser_parse_string(
        parser,
        NULL,
        source_code,
        std::strlen(source_code)
    );
    std::unique_ptr<char, decltype(free)*> buffer { ts_node_string(ts_tree_root_node(tree)), free };

    std::cout << buffer.get() << std::endl;
}