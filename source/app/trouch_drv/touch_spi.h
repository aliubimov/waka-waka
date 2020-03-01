/* touch_spi.h
 *
 * Copyright (C) 2020 Andrii Liubimov
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */
#ifndef TOUCH_SPI_H_
#define TOUCH_SPI_H_

#include "waka_conf.h"

#include <stdint.h>
#include <stdbool.h>

extern volatile bool in_progress;

typedef struct _touch_reading {
	int16_t x;
	int16_t y;
	int8_t is_pressed;
	uint8_t avg_last;
	int16_t avg_buf_x[XPT2046_AVG];
	int16_t avg_buf_y[XPT2046_AVG];
} touch_reading_t;

void touch_init();

void touch_update_reading(touch_reading_t *last);

#endif /* TOUCH_SPI_H_ */
