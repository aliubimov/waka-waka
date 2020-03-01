/* waka_conf.h
 *
 * Copyright (C) 2020 Andrii Liubimov
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#ifndef WAKA_CONF_H_
#define WAKA_CONF_H_

#define	LCD_FLEXSPI 1
//#define	LCD_LPSPI 1


// TouchScreen IRQ GPIO pin
#define GPIO_PIN_TIRQ 	2


// TouchScreen calibration settings
#define TOUCH_X_MIN		420U
#define TOUCH_Y_MIN		320U
#define TOUCH_X_MAX 	3800U
#define TOUCH_Y_MAX		3800U

#define XPT2046_AVG	    4


#endif /* WAKA_CONF_H_ */
