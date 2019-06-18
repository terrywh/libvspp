#ifndef INCLUDE_TOML__INTERNAL_H_
#define INCLUDE_TOML__INTERNAL_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct toml__internal_s toml__internal_t;
struct toml__internal_s {
  int32_t _index;
  void* _span_pos0;
  void* _span_cb0;
  int32_t error;
  const char* reason;
  const char* error_pos;
  void* data;
  void* _current;
  void* settings;
};

int toml__internal_init(toml__internal_t* s);
int toml__internal_execute(toml__internal_t* s, const char* p, const char* endp);

#ifdef __cplusplus
}  /* extern "C" */
#endif
#endif  /* INCLUDE_TOML__INTERNAL_H_ */
