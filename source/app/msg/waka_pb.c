#include <stdlib.h>
#include <strings.h>

#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

bool encode_va_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    const char *str = (char*) *arg;
    if (!pb_encode_tag_for_field(stream, field))
        return false;

    return pb_encode_string(stream, (uint8_t*)str, strlen(str));
}

bool decode_va_string(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    pb_byte_t* buffer = (pb_byte_t*) malloc(stream->bytes_left + 1);
    memset(buffer, 0, stream->bytes_left + 1);

    if (!pb_read(stream, buffer, stream->bytes_left))
        return false;

    *arg = buffer;

    return true;
}
