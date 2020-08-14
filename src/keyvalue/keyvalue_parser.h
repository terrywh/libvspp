#ifndef INCLUDE_KEYVALUE_PARSER_H_
#define INCLUDE_KEYVALUE_PARSER_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum keyvalue_error_e {
    KEYVALUE_PARSER_ERROR_SUCCESS,
    KEYVALUE_PARSER_ERROR_UNEXPECTED_TOKEN = 201,
} keyvalue_error_t;

typedef struct keyvalue_parser_s {
//   int32_t _index;
  void*   _span_pos0;
  void*   _span_cb0;

  int32_t error;
  const char* reason;
  const char* error_pos;
  void* data;
  void* _current;
  // example: "[]={};"
  const char* seperator;
} keyvalue_parser_t;

int keyvalue_parser_init(keyvalue_parser_t* s);
int keyvalue_parser_execute(keyvalue_parser_t* s, const char* p, const char* endp);

#ifdef __cplusplus
}  /* extern "C" */
#endif
#endif  /* INCLUDE_KEYVALUE__H_ */
