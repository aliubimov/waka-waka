/* lora_app.h
 *
 * Copyright (C) 2020 Andrii Liubimov
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#ifndef LORA_APP_H_
#define LORA_APP_H_

#include <stddef.h>

void init_radio();

typedef enum {
	rsNotInitizlied,
	rsUnknown,
	rsSleep,
	rsStandby,
	rsRxInProgress,
	rsTxInProgress
} radio_status_t;


typedef void (*radio_receive_callback_t)(void *data, size_t size);

/*
 * @brief query radio device status
 */
radio_status_t radio_read_status();


/*
 * @brief register given function as callback on radio packet received
 */
void register_receive_callback(radio_receive_callback_t func);


/*
 * @brief unregister callback function
 */
void unregister_receive_callback();

int get_last_packet_rssi();

int get_last_packet_snr();

/*
 * @brief receive radio blocking
 */
// FIXME don't do blocking, it is 21st century
void radio_receive();

#endif /* LORA_APP_H_ */
