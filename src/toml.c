#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "toml.h"

typedef int (*toml__internal__span_cb)(
             toml__internal_t*, const char*, const char*);

enum llparse_state_e {
  s_error,
  s_n_toml__internal__n_begin,
  s_n_toml__internal__n_before_next,
  s_n_toml__internal__n_value_suffix_whitespace,
  s_n_toml__internal__n_value,
  s_n_toml__internal__n_span_start_toml__on_value,
  s_n_toml__internal__n_value_prefix_whitespace,
  s_n_toml__internal__n_equal,
  s_n_toml__internal__n_field_suffix_whitespace,
  s_n_toml__internal__n_field,
  s_n_toml__internal__n_span_start_toml__on_field,
  s_n_toml__internal__n_field_prefix_whitespace,
};
typedef enum llparse_state_e llparse_state_t;

int toml__on_field(
    toml__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int toml__on_value(
    toml__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int toml__on_finish(
    toml__internal_t* s, const unsigned char* p,
    const unsigned char* endp);

int toml__internal_init(toml__internal_t* state) {
  memset(state, 0, sizeof(*state));
  state->_current = (void*) (intptr_t) s_n_toml__internal__n_field_prefix_whitespace;
  return 0;
}

static llparse_state_t toml__internal__run(
    toml__internal_t* state,
    const unsigned char* p,
    const unsigned char* endp) {
  int match;
  switch ((llparse_state_t) (intptr_t) state->_current) {
    case s_n_toml__internal__n_begin:
    s_n_toml__internal__n_begin: {
      if (p == endp) {
        return s_n_toml__internal__n_begin;
      }
      switch (*p) {
        case 0: {
          p++;
          goto s_n_toml__internal__n_invoke_toml__on_finish;
        }
        case ' ': {
          goto s_n_toml__internal__n_field_prefix_whitespace;
        }
        default: {
          goto s_n_toml__internal__n_field_prefix_whitespace;
        }
      }
      /* UNREACHABLE */;
      abort();
    }
    case s_n_toml__internal__n_before_next:
    s_n_toml__internal__n_before_next: {
      if (p == endp) {
        return s_n_toml__internal__n_before_next;
      }
      switch (*p) {
        case 10: {
          p++;
          goto s_n_toml__internal__n_before_next;
        }
        case 13: {
          p++;
          goto s_n_toml__internal__n_before_next;
        }
        default: {
          goto s_n_toml__internal__n_field_prefix_whitespace;
        }
      }
      /* UNREACHABLE */;
      abort();
    }
    case s_n_toml__internal__n_value_suffix_whitespace:
    s_n_toml__internal__n_value_suffix_whitespace: {
      if (p == endp) {
        return s_n_toml__internal__n_value_suffix_whitespace;
      }
      switch (*p) {
        case ' ': {
          p++;
          goto s_n_toml__internal__n_value_suffix_whitespace;
        }
        default: {
          goto s_n_toml__internal__n_before_next;
        }
      }
      /* UNREACHABLE */;
      abort();
    }
    case s_n_toml__internal__n_value:
    s_n_toml__internal__n_value: {
      if (p == endp) {
        return s_n_toml__internal__n_value;
      }
      switch (*p) {
        case 0: {
          goto s_n_toml__internal__n_span_end_toml__on_value;
        }
        case 10: {
          goto s_n_toml__internal__n_span_end_toml__on_value_1;
        }
        case 13: {
          goto s_n_toml__internal__n_span_end_toml__on_value_1;
        }
        case ' ': {
          goto s_n_toml__internal__n_span_end_toml__on_value_2;
        }
        default: {
          p++;
          goto s_n_toml__internal__n_value;
        }
      }
      /* UNREACHABLE */;
      abort();
    }
    case s_n_toml__internal__n_span_start_toml__on_value:
    s_n_toml__internal__n_span_start_toml__on_value: {
      if (p == endp) {
        return s_n_toml__internal__n_span_start_toml__on_value;
      }
      state->_span_pos0 = (void*) p;
      state->_span_cb0 = toml__on_value;
      goto s_n_toml__internal__n_value;
      /* UNREACHABLE */;
      abort();
    }
    case s_n_toml__internal__n_value_prefix_whitespace:
    s_n_toml__internal__n_value_prefix_whitespace: {
      if (p == endp) {
        return s_n_toml__internal__n_value_prefix_whitespace;
      }
      switch (*p) {
        case ' ': {
          p++;
          goto s_n_toml__internal__n_value_prefix_whitespace;
        }
        default: {
          goto s_n_toml__internal__n_span_start_toml__on_value;
        }
      }
      /* UNREACHABLE */;
      abort();
    }
    case s_n_toml__internal__n_equal:
    s_n_toml__internal__n_equal: {
      if (p == endp) {
        return s_n_toml__internal__n_equal;
      }
      switch (*p) {
        case '=': {
          p++;
          goto s_n_toml__internal__n_value_prefix_whitespace;
        }
        default: {
          goto s_n_toml__internal__n_error_1;
        }
      }
      /* UNREACHABLE */;
      abort();
    }
    case s_n_toml__internal__n_field_suffix_whitespace:
    s_n_toml__internal__n_field_suffix_whitespace: {
      if (p == endp) {
        return s_n_toml__internal__n_field_suffix_whitespace;
      }
      switch (*p) {
        case 10: {
          goto s_n_toml__internal__n_before_next;
        }
        case 13: {
          goto s_n_toml__internal__n_before_next;
        }
        case ' ': {
          p++;
          goto s_n_toml__internal__n_field_suffix_whitespace;
        }
        case '=': {
          goto s_n_toml__internal__n_equal;
        }
        default: {
          goto s_n_toml__internal__n_equal;
        }
      }
      /* UNREACHABLE */;
      abort();
    }
    case s_n_toml__internal__n_field:
    s_n_toml__internal__n_field: {
      if (p == endp) {
        return s_n_toml__internal__n_field;
      }
      switch (*p) {
        case 0: {
          goto s_n_toml__internal__n_span_end_toml__on_field;
        }
        case 10: {
          goto s_n_toml__internal__n_span_end_toml__on_field_1;
        }
        case 13: {
          goto s_n_toml__internal__n_span_end_toml__on_field_1;
        }
        case ' ': {
          goto s_n_toml__internal__n_span_end_toml__on_field_2;
        }
        case '=': {
          goto s_n_toml__internal__n_span_end_toml__on_field_3;
        }
        default: {
          p++;
          goto s_n_toml__internal__n_field;
        }
      }
      /* UNREACHABLE */;
      abort();
    }
    case s_n_toml__internal__n_span_start_toml__on_field:
    s_n_toml__internal__n_span_start_toml__on_field: {
      if (p == endp) {
        return s_n_toml__internal__n_span_start_toml__on_field;
      }
      state->_span_pos0 = (void*) p;
      state->_span_cb0 = toml__on_field;
      goto s_n_toml__internal__n_field;
      /* UNREACHABLE */;
      abort();
    }
    case s_n_toml__internal__n_field_prefix_whitespace:
    s_n_toml__internal__n_field_prefix_whitespace: {
      if (p == endp) {
        return s_n_toml__internal__n_field_prefix_whitespace;
      }
      switch (*p) {
        case ' ': {
          p++;
          goto s_n_toml__internal__n_field_prefix_whitespace;
        }
        default: {
          goto s_n_toml__internal__n_span_start_toml__on_field;
        }
      }
      /* UNREACHABLE */;
      abort();
    }
    default:
      /* UNREACHABLE */
      abort();
  }
  s_n_toml__internal__n_error: {
    state->error = 0x1;
    state->reason = "unable to finish";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_error;
    return s_error;
    /* UNREACHABLE */;
    abort();
  }
  s_n_toml__internal__n_invoke_toml__on_finish: {
    switch (toml__on_finish(state, p, endp)) {
      case 0:
        goto s_n_toml__internal__n_begin;
      default:
        goto s_n_toml__internal__n_error;
    }
    /* UNREACHABLE */;
    abort();
  }
  s_n_toml__internal__n_span_end_toml__on_field: {
    const unsigned char* start;
    int err;
    
    start = state->_span_pos0;
    state->_span_pos0 = NULL;
    err = toml__on_field(state, start, p);
    if (err != 0) {
      state->error = err;
      state->error_pos = (const char*) p;
      state->_current = (void*) (intptr_t) s_n_toml__internal__n_begin;
      return s_error;
    }
    goto s_n_toml__internal__n_begin;
    /* UNREACHABLE */;
    abort();
  }
  s_n_toml__internal__n_span_end_toml__on_field_1: {
    const unsigned char* start;
    int err;
    
    start = state->_span_pos0;
    state->_span_pos0 = NULL;
    err = toml__on_field(state, start, p);
    if (err != 0) {
      state->error = err;
      state->error_pos = (const char*) p;
      state->_current = (void*) (intptr_t) s_n_toml__internal__n_before_next;
      return s_error;
    }
    goto s_n_toml__internal__n_before_next;
    /* UNREACHABLE */;
    abort();
  }
  s_n_toml__internal__n_span_end_toml__on_value: {
    const unsigned char* start;
    int err;
    
    start = state->_span_pos0;
    state->_span_pos0 = NULL;
    err = toml__on_value(state, start, p);
    if (err != 0) {
      state->error = err;
      state->error_pos = (const char*) p;
      state->_current = (void*) (intptr_t) s_n_toml__internal__n_begin;
      return s_error;
    }
    goto s_n_toml__internal__n_begin;
    /* UNREACHABLE */;
    abort();
  }
  s_n_toml__internal__n_span_end_toml__on_value_1: {
    const unsigned char* start;
    int err;
    
    start = state->_span_pos0;
    state->_span_pos0 = NULL;
    err = toml__on_value(state, start, p);
    if (err != 0) {
      state->error = err;
      state->error_pos = (const char*) p;
      state->_current = (void*) (intptr_t) s_n_toml__internal__n_before_next;
      return s_error;
    }
    goto s_n_toml__internal__n_before_next;
    /* UNREACHABLE */;
    abort();
  }
  s_n_toml__internal__n_span_end_toml__on_value_2: {
    const unsigned char* start;
    int err;
    
    start = state->_span_pos0;
    state->_span_pos0 = NULL;
    err = toml__on_value(state, start, p);
    if (err != 0) {
      state->error = err;
      state->error_pos = (const char*) p;
      state->_current = (void*) (intptr_t) s_n_toml__internal__n_value_suffix_whitespace;
      return s_error;
    }
    goto s_n_toml__internal__n_value_suffix_whitespace;
    /* UNREACHABLE */;
    abort();
  }
  s_n_toml__internal__n_error_1: {
    state->error = 0xa;
    state->reason = "operator '=' expected";
    state->error_pos = (const char*) p;
    state->_current = (void*) (intptr_t) s_error;
    return s_error;
    /* UNREACHABLE */;
    abort();
  }
  s_n_toml__internal__n_span_end_toml__on_field_2: {
    const unsigned char* start;
    int err;
    
    start = state->_span_pos0;
    state->_span_pos0 = NULL;
    err = toml__on_field(state, start, p);
    if (err != 0) {
      state->error = err;
      state->error_pos = (const char*) p;
      state->_current = (void*) (intptr_t) s_n_toml__internal__n_field_suffix_whitespace;
      return s_error;
    }
    goto s_n_toml__internal__n_field_suffix_whitespace;
    /* UNREACHABLE */;
    abort();
  }
  s_n_toml__internal__n_span_end_toml__on_field_3: {
    const unsigned char* start;
    int err;
    
    start = state->_span_pos0;
    state->_span_pos0 = NULL;
    err = toml__on_field(state, start, p);
    if (err != 0) {
      state->error = err;
      state->error_pos = (const char*) p;
      state->_current = (void*) (intptr_t) s_n_toml__internal__n_equal;
      return s_error;
    }
    goto s_n_toml__internal__n_equal;
    /* UNREACHABLE */;
    abort();
  }
}

int toml__internal_execute(toml__internal_t* state, const char* p, const char* endp) {
  llparse_state_t next;

  /* check lingering errors */
  if (state->error != 0) {
    return state->error;
  }

  /* restart spans */
  if (state->_span_pos0 != NULL) {
    state->_span_pos0 = (void*) p;
  }
  
  next = toml__internal__run(state, (const unsigned char*) p, (const unsigned char*) endp);
  if (next == s_error) {
    return state->error;
  }
  state->_current = (void*) (intptr_t) next;

  /* execute spans */
  if (state->_span_pos0 != NULL) {
    int error;
  
    error = ((toml__internal__span_cb) state->_span_cb0)(state, state->_span_pos0, (const char*) endp);
    if (error != 0) {
      state->error = error;
      state->error_pos = endp;
      return error;
    }
  }
  
  return 0;
}