/*
 * lcd_spi.c
 *
 *  Created on: Feb 9, 2020
 *      Author: Andrii
 */
#include <waka_conf.h>

#ifdef LCD_FLEXSPI

#include "fsl_gpio.h"
#include "fsl_edma.h"

#include "peripherals.h"

volatile bool in_progress = false;

const gpio_pin_config_t dcx_config = {
		.direction = kGPIO_DigitalOutput
};

#define GPIO_PIN_RESET	1
#define GPIO_PIN_CDX	2

#include "fsl_flexio_spi.h"

static flexio_spi_master_handle_t flexio_spi_handle;

void FLEXSPI_MasterUserCallback(FLEXIO_SPI_Type *base, flexio_spi_master_handle_t *handle, status_t status, void *userData)
{
    in_progress = false;
}

void init_dma() {
 	FLEXIO_SPI_MasterTransferCreateHandle(&FLEXIO1_peripheralConfig, &flexio_spi_handle, FLEXSPI_MasterUserCallback, NULL);
	in_progress = false;
}

void init_dcx() {
	GPIO_PinInit(GPIO1, GPIO_PIN_CDX, &dcx_config);
	GPIO_PinInit(GPIO1, GPIO_PIN_RESET, &dcx_config);
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
	FLEXIO_SPI_MasterTransferNonBlocking(&FLEXIO1_peripheralConfig, &flexio_spi_handle, &xfer);
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
	FLEXIO_SPI_MasterTransferNonBlocking(&FLEXIO1_peripheralConfig, &flexio_spi_handle, &xfer);
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
	FLEXIO_SPI_MasterTransferNonBlocking(&FLEXIO1_peripheralConfig, &flexio_spi_handle, &xfer);
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
	FLEXIO_SPI_MasterTransferNonBlocking(&FLEXIO1_peripheralConfig, &flexio_spi_handle, &xfer_cmd);

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
	FLEXIO_SPI_MasterTransferNonBlocking(&FLEXIO1_peripheralConfig, &flexio_spi_handle, &xfer_data);
}

void write_reset(uint8_t val)
{
	GPIO_PinWrite(GPIO1, GPIO_PIN_RESET, val);
}

#endif
