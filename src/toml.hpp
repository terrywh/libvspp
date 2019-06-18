#ifndef INCLUDE_TOML_PARSER_HPP
#define INCLUDE_TOML_PARSER_HPP

#include "toml.h"

typedef struct toml__internal_s toml_parser_t;
struct toml__internal_settings_s {};
typedef struct toml__internal_settings_s toml_parser_settings_t;

class toml_parser {
public:
    toml_parser_t           parser_;
    toml_parser_settings_t setting_;
};

#endif  /* INCLUDE_TOML_PARSER_HPP */
