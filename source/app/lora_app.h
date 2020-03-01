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

void init_radio();

uint8_t radio_read_reg(uint8_t reg);

#endif /* LORA_APP_H_ */
