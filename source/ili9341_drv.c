/* ili9341.c
 *
 * Copyright (C) 2020 Andrii Liubimov
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include "ili9341_drv.h"


extern void usleep(uint32_t ms);


void ili9341_init_default(ili9341_handle_t *handle)
{
	handle->write_cmd = NULL;
	handle->write_data = NULL;
	handle->write_data_dma = NULL;
	handle->write_reg8 = NULL;
	handle->write_reg16 = NULL;
}

void ili3941_soft_reset(ili9341_handle_t *handle) {
	handle->write_cmd(ILI9341_SWRESET);
}


void ili9341_init_controller(ili9341_handle_t *handle)
{
	handle->write_reg8(ILI9341_PIXFMT, 0x55);
	handle->write_reg8(ILI9341_MADCTL, 0xe8);
}

void ili9341_screen_on(ili9341_handle_t *handle)
{
	handle->write_reg8(ILI9341_SLPOUT, 0x80);
	handle->write_reg8(ILI9341_DISPON, 0x80);

    usleep(150000);
}

void ili9341_write_buffer(ili9341_handle_t *handle, uint8_t buf, size_t buf_size)
{
	handle->write_cmd(0x3c);
	handle->write_data_dma(&buf, buf_size);
}

void ili9341_hard_reset(ili9341_handle_t *handle)
{
    handle->write_reset(1);
    usleep(150000U);
    handle->write_reset(0);
    usleep(150000U);
    handle->write_reset(1);
    usleep(150000U);
}
