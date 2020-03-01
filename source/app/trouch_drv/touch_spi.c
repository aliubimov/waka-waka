/* touch_spi.c
 *
 * Copyright (C) 2020 Andrii Liubimov
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */
#include "waka_conf.h"
#include "touch_spi.h"

#include "fsl_gpio.h"
#include "fsl_edma.h"
#include "fsl_lpspi.h"
#include "fsl_lpspi_edma.h"

#include "peripherals.h"

void touch_init()
{
	gpio_pin_config_t touch_irq_config = {
			.direction = kGPIO_DigitalInput
	};

	GPIO_PinInit(GPIO1, GPIO_PIN_TIRQ, &touch_irq_config);

}

static void xpt2046_avg(touch_reading_t* reading)
{
    /*Shift out the oldest data*/
    uint8_t i;
    for(i = XPT2046_AVG - 1; i > 0 ; i--) {
    	reading->avg_buf_x[i] = reading->avg_buf_x[i - 1];
    	reading->avg_buf_y[i] = reading->avg_buf_y[i - 1];
    }

    /*Insert the new point*/
    reading->avg_buf_x[0] = reading->x;
    reading->avg_buf_y[0] = reading->y;

    if(reading->avg_last < XPT2046_AVG) reading->avg_last++;

    /*Sum the x and y coordinates*/
    int32_t x_sum = 0;
    int32_t y_sum = 0;

    for(i = 0; i < reading->avg_last ; i++) {
        x_sum += reading->avg_buf_x[i];
        y_sum += reading->avg_buf_y[i];
    }

    /*Normalize the sums*/
    reading->x = (int32_t)x_sum / reading->avg_last;
    reading->y = (int32_t)y_sum / reading->avg_last;
}

void touch_update_reading(touch_reading_t *last)
{
	last->is_pressed = ! GPIO_PinRead(GPIO1, GPIO_PIN_TIRQ);

	if (last->is_pressed) {
		while (in_progress) {
			__NOP();
		};

		LPSPI1_txBuffer[0] = 0b10010000;
		LPSPI1_txBuffer[1] = 0x00;
		LPSPI1_txBuffer[2] = 0b11010000;
		LPSPI1_txBuffer[3] = 0x00;

		LPSPI1_transfer.dataSize = 5;
		LPSPI1_transfer.configFlags = kLPSPI_MasterPcsContinuous | kLPSPI_MasterPcs1;

		while (LPSPI_GetStatusFlags(LPSPI1) & kLPSPI_ModuleBusyFlag) {};
		assert (LPSPI_MasterTransferBlocking(LPSPI1, &LPSPI1_transfer) == kStatus_Success);

		uint16_t x = ((LPSPI1_rxBuffer[1] << 8) | LPSPI1_rxBuffer[2]) >> 3;
		uint16_t y = ((LPSPI1_rxBuffer[3] << 8) | LPSPI1_rxBuffer[4]) >> 3;

		if (x > TOUCH_X_MIN)
		{
			x -= TOUCH_X_MIN;
		} else {
			x = 0;
		}
		if (y > TOUCH_Y_MIN)
		{
			y -= TOUCH_Y_MIN;
		} else {
			y = 0;
		}

		x = (x * 320) / (TOUCH_X_MAX - TOUCH_X_MIN);
		y = (y * 240) / (TOUCH_Y_MAX - TOUCH_Y_MIN);

		if (x > 320) {
			x = 319;
		}

		if (y > 240) {
			y = 239;
		}

		xpt2046_avg(last);

		last->x = x;
		last->y = y;
	} else {
		last->avg_last = 0;
	}
}
