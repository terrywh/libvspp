#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "keyvalue.h"

typedef enum keyvalue_parser_status_e {
    keyvalue_parser_error = 0,
    keyvalue_parser_status_begin = 0,
    keyvalue_parser_status_before_field,
    keyvalue_parser_status_field_start,
    keyvalue_parser_status_field,
    keyvalue_parser_status_field_end,
    keyvalue_parser_status_after_field,
    keyvalue_parser_status_equal,
    keyvalue_parser_status_before_value,
    keyvalue_parser_status_value_start,
    keyvalue_parser_status_value,
    keyvalue_parser_status_value_end,
    keyvalue_parser_status_after_value,
    keyvalue_parser_status_delimiter,
} keyvalue_parser_status_t;

typedef enum keyvalue_parser_seperator_e {
    keyvalue_parser_seperator_before_field,
    keyvalue_parser_seperator_after_field,
    keyvalue_parser_seperator_equal,
    keyvalue_parser_seperator_before_value,
    keyvalue_parser_seperator_after_value,
    keyvalue_parser_seperator_delimiter,
    keyvalue_parser_seperator_max,
} keyvalue_parser_seperator_t;

typedef int (*keyvalue_parser_span_cb)(keyvalue_parser_t* state, const unsigned char* p, const unsigned char* endp);

int keyvalue_parser_field(keyvalue_parser_t* state, const unsigned char* p, const unsigned char* endp);
int keyvalue_parser_value(keyvalue_parser_t* state, const unsigned char* p, const unsigned char* endp);

int keyvalue_parser_init(keyvalue_parser_t* state) {
    memset(state, 0, sizeof(keyvalue_parser_t));
    return 0;
}

inline static int keyvalue_parser_is_seperator(keyvalue_parser_t* state, keyvalue_parser_seperator_t s, const unsigned char* i) {
    return *i == state->seperator[s];
}

inline static int keyvalue_parser_if_seperator(keyvalue_parser_t* state, keyvalue_parser_seperator_t s) {
    return state->seperator[s] != '\0';
}
#define span_start(span) do {          \
    state->_span_cb0  = span;          \
    state->_span_pos0 = (void*)i;      \
} while(0)
#define span_end(span) do {            \
    span(state, state->_span_pos0, i); \
    state->_span_cb0  = NULL;          \
    state->_span_pos0 = NULL;          \
} while(0)

int32_t keyvalue_parser_run(keyvalue_parser_t* state, const unsigned char* p, const unsigned char* endp) {
    for(const unsigned char* i = p; i!=endp; ++i) {
        switch((uintptr_t)state->_current) {
        case keyvalue_parser_status_begin:
            state->_current = (void*)(uintptr_t)keyvalue_parser_status_before_field;
            // 本次数据需要立即进行处理
        case keyvalue_parser_status_before_field:
        keyvalue_parser_status_before_field:
            if(keyvalue_parser_if_seperator(state, keyvalue_parser_seperator_before_field)) {
                if(keyvalue_parser_is_seperator(state, keyvalue_parser_seperator_before_field, i))
                    state->_current = (void*)(uintptr_t)keyvalue_parser_status_field_start;
            }
            else if(!isspace(*i)) {
                state->_current = (void*)(uintptr_t)keyvalue_parser_status_field_start;
                goto keyvalue_parser_status_field_start; // 本次数据需要立即进行处理
            }
            break;
        case keyvalue_parser_status_field_start:
        keyvalue_parser_status_field_start:
            span_start(keyvalue_parser_field);
            state->_current = (void*)(uintptr_t)keyvalue_parser_status_field;
            // 本次数据需要立即进行处理
        case keyvalue_parser_status_field:
        keyvalue_parser_status_field:
            if(keyvalue_parser_if_seperator(state, keyvalue_parser_seperator_after_field)) {
                if(keyvalue_parser_is_seperator(state, keyvalue_parser_seperator_after_field, i)) {
                    span_end(keyvalue_parser_field);
                    state->_current = (void*)(uintptr_t)keyvalue_parser_status_after_field;
                }
            }
            else if(isspace(*i)) {
                span_end(keyvalue_parser_field);
                state->_current = (void*)(uintptr_t)keyvalue_parser_status_after_field;
            }
            else if(keyvalue_parser_is_seperator(state, keyvalue_parser_seperator_equal, i)) {
                span_end(keyvalue_parser_field);
                state->_current = (void*)(uintptr_t)keyvalue_parser_status_equal;
                goto keyvalue_parser_status_equal; // 本次数据需要立即进行处理
            }
            break;
        case keyvalue_parser_status_after_field:
        keyvalue_parser_status_after_field:
            if(keyvalue_parser_is_seperator(state, keyvalue_parser_seperator_equal, i)) {
                state->_current = (void*)(uintptr_t)keyvalue_parser_status_equal;
                goto keyvalue_parser_status_equal; // 本次数据需要立即进行处理
            }
            break;
        case keyvalue_parser_status_equal:
        keyvalue_parser_status_equal:
            if(!keyvalue_parser_is_seperator(state, keyvalue_parser_seperator_equal, i)) {
                state->reason = "unexpected token";
                state->error  = 201;
                state->error_pos = (const char*) i;
                goto keyvalue_parser_error;
            }
            state->_current = (void*)(uintptr_t)keyvalue_parser_status_before_value;
            break;
        case keyvalue_parser_status_before_value:
        keyvalue_parser_status_before_value:
            if(keyvalue_parser_if_seperator(state,keyvalue_parser_seperator_before_value)) {
                if(keyvalue_parser_is_seperator(state, keyvalue_parser_seperator_before_value, i))
                    state->_current = (void*)(uintptr_t)keyvalue_parser_status_value_start;
            }
            else if(!isspace(*i)) {
                state->_current = (void*)(uintptr_t)keyvalue_parser_status_value_start;
                goto keyvalue_parser_status_value_start; // 本次数据需要立即进行处理
            }
            break;
        case keyvalue_parser_status_value_start:
        keyvalue_parser_status_value_start:
            span_start(keyvalue_parser_value);
            state->_current = (void*)(uintptr_t)keyvalue_parser_status_value;
            // 本次数据需要立即进行处理
        case keyvalue_parser_status_value:
        keyvalue_parser_status_value:
            if(keyvalue_parser_if_seperator(state, keyvalue_parser_seperator_after_value)) {
                if(keyvalue_parser_is_seperator(state, keyvalue_parser_seperator_after_value, i)) {
                    span_end(keyvalue_parser_value);
                    state->_current = (void*)(uintptr_t)keyvalue_parser_status_after_value;
                }
            }
            else if(isspace(*i)) {
                span_end(keyvalue_parser_value);
                state->_current = (void*)(uintptr_t)keyvalue_parser_status_after_value;
            }
            else if(keyvalue_parser_is_seperator(state, keyvalue_parser_seperator_delimiter, i)) {
                span_end(keyvalue_parser_value);
                state->_current = (void*)(uintptr_t)keyvalue_parser_status_delimiter;
                goto keyvalue_parser_status_delimiter; // 本次数据需要立即进行处理
            }
            break;
        case keyvalue_parser_status_after_value:
        keyvalue_parser_status_after_value:
            if(!isspace(*i)) {
                state->_current = (void*)(uintptr_t)keyvalue_parser_status_delimiter;
                goto keyvalue_parser_status_delimiter;
            }
            break;
        case keyvalue_parser_status_delimiter:
        keyvalue_parser_status_delimiter:
            if(!keyvalue_parser_is_seperator(state, keyvalue_parser_seperator_delimiter, i)) {
                state->reason = "delimiter";
                state->error  = KEYVALUE_PARSER_ERROR_UNEXPECTED_TOKEN;
                state->error_pos = (const char*) i;
                goto keyvalue_parser_error;
            }
            state->_current = (void*)(uintptr_t)keyvalue_parser_status_before_field;
            break;
        default:
            abort(); // 
        }
    }
    

    return (uintptr_t)state->_current;
keyvalue_parser_error:
    return keyvalue_parser_error;
}

int keyvalue_parser_execute(keyvalue_parser_t* state, const char* p, const char* endp) {
    /* check lingering errors */
    if (state->error != 0) {
        return state->error;
    }
    /* restart spans */
    if (state->_span_pos0 != NULL) {
        state->_span_pos0 = (void*) p;
    }
    int32_t next = keyvalue_parser_run(state, (const unsigned char*) p, (const unsigned char*) endp);

    if (next == 0) {
        return state->error;
    }
    state->_current = (void*) (intptr_t) next;
    /* execute spans */
    if (state->_span_pos0 != NULL) {
        int error;
    
        error = ((keyvalue_parser_span_cb) state->_span_cb0)(state, state->_span_pos0, (const unsigned char*) endp);
        if (error != 0) {
            state->error = error;
            state->error_pos = endp;
            return error;
        }
    }
    return 0;
}
