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


#define GPIO_PIN_RESET	1
#define GPIO_PIN_CDX	2


lpspi_master_edma_handle_t g_m_edma_handle;
edma_handle_t lpspiEdmaMasterRxRegToRxDataHandle;
edma_handle_t lpspiEdmaMasterTxDataToTxRegHandle;

volatile bool in_progress = false;

const gpio_pin_config_t dcx_config = {
		.direction = kGPIO_DigitalOutput
};

void LPSPI_MasterUserCallback(LPSPI_Type *base, lpspi_master_edma_handle_t *handle, status_t status, void *userData)
{
    in_progress = false;
}

void init_dma() {
    memset(&(lpspiEdmaMasterRxRegToRxDataHandle), 0, sizeof(lpspiEdmaMasterRxRegToRxDataHandle));
    memset(&(lpspiEdmaMasterTxDataToTxRegHandle), 0, sizeof(lpspiEdmaMasterTxDataToTxRegHandle));

	EDMA_CreateHandle(&lpspiEdmaMasterTxDataToTxRegHandle, DMA0, DMA0_CH0_DMA_CHANNEL);
	EDMA_CreateHandle(&lpspiEdmaMasterRxRegToRxDataHandle, DMA0, DMA0_CH1_DMA_CHANNEL);

	LPSPI_MasterTransferCreateHandleEDMA(LPSPI1, &g_m_edma_handle, LPSPI_MasterUserCallback, NULL, &lpspiEdmaMasterRxRegToRxDataHandle, &lpspiEdmaMasterTxDataToTxRegHandle);
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

	GPIO_PinWrite(GPIO1, GPIO_PIN_CDX, 0);

	LPSPI1_txBuffer[0] = cmd;
    LPSPI1_transfer.dataSize = 1;
    LPSPI1_transfer.configFlags = kLPSPI_MasterPcsContinuous | kLPSPI_MasterPcs0;
    while (LPSPI_GetStatusFlags(LPSPI1) & kLPSPI_ModuleBusyFlag) {};
    assert (LPSPI_MasterTransferBlocking(LPSPI1, &LPSPI1_transfer) == kStatus_Success);

    GPIO_PinWrite(GPIO1, GPIO_PIN_CDX, 1);
}

void write_data(uint8_t data)
{
	while (in_progress) {
		__WFI();
	}

	GPIO_PinWrite(GPIO1, GPIO_PIN_CDX, 1);

	LPSPI1_txBuffer[0] = data;
    LPSPI1_transfer.dataSize = 1;
    LPSPI1_transfer.configFlags = kLPSPI_MasterPcsContinuous | kLPSPI_MasterPcs0;
    while (LPSPI_GetStatusFlags(LPSPI1) & kLPSPI_ModuleBusyFlag) {};
    assert (LPSPI_MasterTransferBlocking(LPSPI1, &LPSPI1_transfer) == kStatus_Success);
}

void write_data_dma(uint8_t *data, size_t size)
{
	lpspi_transfer_t transfer;

	transfer.txData = data;
	transfer.rxData = NULL;
	transfer.dataSize = size;
	transfer.configFlags = kLPSPI_MasterPcsContinuous | kLPSPI_MasterPcs0;

	while (in_progress) {
		__WFI();
	}

	in_progress = true;

	GPIO_PinWrite(GPIO1, GPIO_PIN_CDX, 1);
	LPSPI_MasterTransferEDMA(LPSPI1, &g_m_edma_handle, &transfer);
}

void write_reg8(uint8_t cmd, uint8_t data)
{
	while (in_progress) {
		__WFI();
	}

	GPIO_PinWrite(GPIO1, GPIO_PIN_CDX, 0);

	LPSPI1_txBuffer[0] = cmd;
    LPSPI1_transfer.dataSize = 1;
    LPSPI1_transfer.configFlags = kLPSPI_MasterPcsContinuous | kLPSPI_MasterPcs0;
    while (LPSPI_GetStatusFlags(LPSPI1) & kLPSPI_ModuleBusyFlag) {};
    assert (LPSPI_MasterTransferBlocking(LPSPI1, &LPSPI1_transfer) == kStatus_Success);

	GPIO_PinWrite(GPIO1, GPIO_PIN_CDX, 1);

	LPSPI1_txBuffer[0] = data;
    LPSPI1_transfer.dataSize = 1;
    LPSPI1_transfer.configFlags = kLPSPI_MasterPcsContinuous | kLPSPI_MasterPcs0;
    while (LPSPI_GetStatusFlags(LPSPI1) & kLPSPI_ModuleBusyFlag) {};
    assert (LPSPI_MasterTransferBlocking(LPSPI1, &LPSPI1_transfer) == kStatus_Success);
}

void write_reset(uint8_t val)
{
	GPIO_PinWrite(GPIO1, GPIO_PIN_RESET, val);
}
