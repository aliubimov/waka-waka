/*
 * ili9341_drv.c
 *
 *  Created on: Feb 1, 2020
 *      Author: Andrii
 */

#include <ili9341_drv.h>

static const uint8_t initcmd[] = {
//  0xEF, 3, 0x03, 0x80, 0x02,
//  0xCF, 3, 0x00, 0xC1, 0x30,
//  0xED, 4, 0x64, 0x03, 0x12, 0x81,
//  0xE8, 3, 0x85, 0x00, 0x78,
//  0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
//  0xF7, 1, 0x20,
//  0xEA, 2, 0x00, 0x00,
//  ILI9341_PWCTR1  , 1, 0x23,             // Power control VRH[5:0]
//  ILI9341_PWCTR2  , 1, 0x10,             // Power control SAP[2:0];BT[3:0]
//  ILI9341_VMCTR1  , 2, 0x3e, 0x28,       // VCM control
//  ILI9341_VMCTR2  , 1, 0x86,             // VCM control2
  ILI9341_MADCTL  , 1, 0xe8,             // Memory Access Control
//  ILI9341_VSCRSADD, 2, 0x01, 0x40,              // Vertical scroll zero
  ILI9341_PIXFMT  , 1, 0x55,
  0x2b, 4, 0x00, 0x00, 0x00, 0xf0,
  0x2a, 4, 0x00, 0x00, 0x01, 0x40,
//  ILI9341_FRMCTR1 , 2, 0x00, 0x18,
//  ILI9341_DFUNCTR , 3, 0x08, 0x82, 0x27, // Display Function Control
//  0xF2, 1, 0x00,                         // 3Gamma Function Disable
//  ILI9341_GAMMASET , 1, 0x01,             // Gamma curve selected
//  ILI9341_GMCTRP1 , 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
//    0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
//  ILI9341_GMCTRN1 , 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
//    0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
// ILI9341_SLPOUT  , 0x80,                // Exit Sleep
//  ILI9341_DISPON  , 0x80,                // Display on
  0x00                                   // End of list
};


void ili9341_init_default(ili9341_handle_t *handle)
{

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
}
