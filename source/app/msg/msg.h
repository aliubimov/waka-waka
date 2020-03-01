/* lora_app.h
 *
 * Copyright (C) 2020 Andrii Liubimov
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#ifndef MSG_MSG_H_
#define MSG_MSG_H_

#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct _msg {
	size_t device_name_size;
	char * device_name;

	size_t msg_size;
	char * msg;
} msg_t;


typedef struct _msg_encoded {
	size_t buf_size;
	char* buf;
} msg_encoded_t;

msg_t* alloc_msg();
msg_encoded_t* alloc_msg_buf();

void free_msg_buf(msg_encoded_t* msg);
void free_msg(msg_t* msg);

msg_encoded_t* encode_msg(msg_t *msg);

msg_t* decode_msg(msg_encoded_t *msg);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* MSG_MSG_H_ */
