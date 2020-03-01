/* msg.c
 *
 * Copyright (C) 2020 Andrii Liubimov
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include "waka_conf.h"
#include "msg.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

msg_t* alloc_msg()
{
	return malloc(sizeof(msg_t));
}

msg_encoded_t* alloc_msg_buf()
{
	return malloc(sizeof(msg_encoded_t));
}

void free_msg_buf(msg_encoded_t* msg)
{
	free(msg);
}

void free_msg(msg_t* msg)
{
	free(msg);
}


void create_msg(msg_t* msg, size_t device_name_size, const char *device_name, size_t msg_size, const char *txt)
{
	msg->device_name_size = device_name_size;
	msg->device_name = malloc(device_name_size);
	snprintf(msg->device_name, device_name_size, device_name);

	msg->msg_size = msg_size;
	msg->device_name = malloc(msg_size);
	snprintf(msg->device_name, msg_size, txt);
}

msg_encoded_t* encode_msg(msg_t *msg)
{
	msg_encoded_t* r = alloc_msg_buf();
	r->buf = malloc(msg->device_name_size + msg->msg_size + 2 * sizeof(size_t));

	int p = 0;

	r->buf[p] = msg->device_name_size;
	p += sizeof(size_t);

	memcpy(&r->buf[p], msg->device_name, msg->device_name_size);
	p += msg->device_name_size;

	r->buf[p] = msg->msg_size;
	p += sizeof(size_t);

	memcpy(&r->buf[p], msg->device_name, msg->msg_size);
	p += msg->msg_size;

	r->buf_size = p;

	return r;
}

msg_t* decode_msg(msg_encoded_t *msg)
{
	msg_t* r = alloc_msg();

	size_t p = 0;

	r->device_name_size = msg->buf[p++];
	memcpy(r->device_name, &msg->buf[p], r->device_name_size);
	p += r->device_name_size;

	r->msg_size = msg->buf[p++];
	memcpy(r->msg, &msg->buf[p], r->msg_size);
	p += r->msg_size;

	return r;
}
