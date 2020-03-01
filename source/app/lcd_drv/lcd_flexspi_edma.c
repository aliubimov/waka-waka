/* lcd_flexspi_edma.c
 *
 * Copyright (C) 2020 Andrii Liubimov
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */
#include <waka_conf.h>

#ifdef LCD_FLEXSPI_EDMA

#include "fsl_gpio.h"
#include "fsl_edma.h"

#include "peripherals.h"

volatile bool in_progress = false;

const gpio_pin_config_t dcx_config = {
		.direction = kGPIO_DigitalOutput
};

#define GPIO_PIN_RESET	1
#define GPIO_PIN_CDX	2

#include "fsl_flexio.h"
#include "fsl_flexio_spi.h"
#include "fsl_flexio_spi_edma.h"

const flexio_timer_config_t timerConfig = {
    .triggerSelect = FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(1),
	.triggerSource = kFLEXIO_TimerTriggerSourceExternal,
	.timerOutput = kFLEXIO_TimerOutputZeroNotAffectedByReset,
	.timerMode = kFLEXIO_TimerModeDual8BitBaudBit,
	.timerCompare = 0x0f1d,
	.timerReset = kFLEXIO_TimerResetNever,
	.timerDecrement = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput,
	.timerEnable = kFLEXIO_TimerEnableOnTriggerHigh,
	.timerDisable = kFLEXIO_TimerDisableOnTimerCompare,
	.timerStop = kFLEXIO_TimerStopBitDisabled,
	.timerStart = kFLEXIO_TimerStartBitDisabled,
	.pinPolarity = kFLEXIO_PinActiveLow,
	.pinSelect = 26,
	.pinConfig = kFLEXIO_PinConfigOutput
};

flexio_shifter_config_t shifterConfig = {
		.timerSelect = 0,
		.timerPolarity = kFLEXIO_ShifterTimerPolarityOnPositive,
		.pinConfig = kFLEXIO_PinConfigOutputDisabled,
		.pinPolarity = kFLEXIO_PinActiveLow,
		.shifterMode = kFLEXIO_ShifterModeTransmit,
		.inputSource = kFLEXIO_ShifterInputFromNextShifterOutput,
		.shifterStop = kFLEXIO_ShifterStopBitHigh,
		.shifterStart = kFLEXIO_ShifterStartBitLow
};

void FLEX_SPI_MasterUserCallback(FLEXIO_SPI_Type *base, flexio_spi_master_edma_handle_t *handle, status_t status, void *userData)
{
    in_progress = false;
}

void init_dma() {
	in_progress = false;
}

void init_dcx() {
	GPIO_PinInit(GPIO1, GPIO_PIN_RESET, &dcx_config);
	GPIO_PinInit(GPIO1, GPIO_PIN_CDX, &dcx_config);
}


void write_cmd(uint8_t cmd)
{
	while (in_progress) {
		__WFI();
	}

	flexio_spi_transfer_t xfer = {
	    .txData = &cmd,
		.rxData = NULL,
		.dataSize = 1,
	    .flags = kFLEXIO_SPI_8bitMsb
	};

	in_progress = true;
	GPIO_PinWrite(GPIO1, GPIO_PIN_CDX, 0);
	FLEXIO_SPI_MasterTransferEDMA(&FLEXIO1_peripheralConfig, &FLEXIO1_eDMA_Handle, &xfer);

	while (in_progress) {
		__WFI();
	}
}

void write_data(uint8_t data)
{
	while (in_progress) {
		__WFI();
	}

	flexio_spi_transfer_t xfer = {
	    .txData = &data,
		.rxData = NULL,
		.dataSize = 1,
	    .flags = kFLEXIO_SPI_8bitMsb
	};

	in_progress = true;
	GPIO_PinWrite(GPIO1, GPIO_PIN_CDX, 1);
	FLEXIO_SPI_MasterTransferEDMA(&FLEXIO1_peripheralConfig, &FLEXIO1_eDMA_Handle, &xfer);

	while (in_progress) {
		__WFI();
	}
}

void write_data_dma(uint8_t *data, size_t size)
{
	while (in_progress) {
		__WFI();
	}

	flexio_spi_transfer_t xfer = {
	    .txData = data,
		.rxData = NULL,
		.dataSize = size,
	    .flags = kFLEXIO_SPI_8bitMsb
	};

	in_progress = true;
	GPIO_PinWrite(GPIO1, GPIO_PIN_CDX, 1);
	FLEXIO_SPI_MasterTransferEDMA(&FLEXIO1_peripheralConfig, &FLEXIO1_eDMA_Handle, &xfer);

	while (in_progress) {
		__WFI();
	}
}

void write_reg8(uint8_t cmd, uint8_t data)
{
	while (in_progress) {
		__WFI();
	}

	flexio_spi_transfer_t xfer_cmd = {
	    .txData = &cmd,
		.rxData = NULL,
		.dataSize = 1,
	    .flags = kFLEXIO_SPI_8bitMsb
	};

	in_progress = true;
	GPIO_PinWrite(GPIO1, GPIO_PIN_CDX, 0);
	FLEXIO_SPI_MasterTransferEDMA(&FLEXIO1_peripheralConfig, &FLEXIO1_eDMA_Handle, &xfer_cmd);


	while (in_progress) {
		__WFI();
	}

	flexio_spi_transfer_t xfer_data = {
	    .txData = &data,
		.rxData = NULL,
		.dataSize = 1,
	    .flags = kFLEXIO_SPI_8bitMsb
	};

	in_progress = true;
	GPIO_PinWrite(GPIO1, GPIO_PIN_CDX, 1);
	FLEXIO_SPI_MasterTransferEDMA(&FLEXIO1_peripheralConfig, &FLEXIO1_eDMA_Handle, &xfer_data);

	while (in_progress) {
		__WFI();
	}
}

void write_reset(uint8_t val)
{
	GPIO_PinWrite(GPIO1, GPIO_PIN_RESET, val);
}

#endif
