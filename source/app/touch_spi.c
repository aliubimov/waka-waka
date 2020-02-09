/*
 * lcd_spi.c
 *
 *  Created on: Feb 9, 2020
 *      Author: Andrii
 */

#include "fsl_gpio.h"
#include "fsl_edma.h"
#include "fsl_lpspi.h"
#include "fsl_lpspi_edma.h"

#include "peripherals.h"


#define GPIO_PIN_TIRQ 	28

#define TOUCH_X_MIN		420U
#define TOUCH_Y_MIN		320U
#define TOUCH_X_MAX 	3800U
#define TOUCH_Y_MAX		3800U


extern volatile bool in_progress;

