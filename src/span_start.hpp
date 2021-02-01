#ifndef VSP_SPAN_START_H
#define VSP_SPAN_START_H

#define SPAN_START(state) do { \
        state_ = state; \
        span_ = i; \
    } while(0)

#define SPAN_END(state, callback) do { \
        state_ = state; \
        callback( data.substr(span_, i - span_) ); \
        span_ = -1; \
    } while(0)

#endif // VSP_SPAN_START_H