#ifndef WAKA_PB_H_INCLUDED
#define WAKA_PB_H_INCLUDED

#include "pb.h"

#if defined (__cplusplus)
extern "C" {
#endif

bool encode_va_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg);

bool decode_va_string(pb_istream_t *stream, const pb_field_t *field, void **arg);

#if defined (__cplusplus)
}
#endif /* __cplusplus */
#endif /* WAKA_PB_H_INCLUDED */
