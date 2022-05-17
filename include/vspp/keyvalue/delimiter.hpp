#ifndef VSPP_KEYVALUE_DELIMITER_H
#define VSPP_KEYVALUE_DELIMITER_H

#include <cstring> // for memcpy

namespace vspp { namespace keyvalue {
    struct delimiter {
        delimiter(const char* s) {
            std::memcpy(this, s, sizeof(delimiter));
        }
        char before_field;
        char  after_field;
        char     operater;
        char before_value;
        char  after_value;
        char    seperator;
        char       rev[2]; // 对齐 8 字节
    };
}}

#endif // VSP_KEYVALUE_DELIMITER_H
