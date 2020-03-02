/* lora_app.c
 *
 * Copyright (C) 2020 Andrii Liubimov
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include "lora_app.h"

#include "lora_drv/lora_drv.h"
#include "drivers/fsl_lpspi.h"
#include "drivers/fsl_lpspi_edma.h"
#include "peripherals.h"

#ifdef DEBUG
#include "fsl_debug_console.h"
#endif


#define RFM_STATUS_REG		0x01


extern volatile bool in_progress;

static void lora_write_reg8(uint8_t reg, uint8_t data);
static void lora_write_dma(uint8_t reg, uint8_t *data, size_t size);
static uint8_t lora_read_reg8(uint8_t reg);

static lora_dev_t lora_dev = {
	    .write_reg8 = lora_write_reg8,
	    .write_reg_dma = lora_write_dma,
	    .read_reg8 = lora_read_reg8
};

static lpspi_master_edma_handle_t g_m_edma_handle;
static edma_handle_t lpspiEdmaMasterRxRegToRxDataHandle;
static edma_handle_t lpspiEdmaMasterTxDataToTxRegHandle;

static volatile radio_receive_callback_t callback = NULL;

static void lora_write_reg8(uint8_t reg, uint8_t data)
{
	while (in_progress) {
		__WFI();
	}

	LPSPI1_txBuffer[0] = reg | 0x80;
	LPSPI1_txBuffer[1] = data;

    LPSPI1_transfer.dataSize = 2;
    LPSPI1_transfer.configFlags = kLPSPI_MasterPcsContinuous | kLPSPI_MasterPcs0;

	in_progress = true;

	while (LPSPI_GetStatusFlags(LPSPI1) & kLPSPI_ModuleBusyFlag) {};
    assert (LPSPI_MasterTransferBlocking(LPSPI1, &LPSPI1_transfer) == kStatus_Success);

	in_progress = false;

}

static void lora_write_dma(uint8_t reg, uint8_t *data, size_t size) {

}

static uint8_t lora_read_reg8(uint8_t reg) {
	while (in_progress) {
		__WFI();
	}

	LPSPI1_txBuffer[0] = reg;
	LPSPI1_txBuffer[1] = 0x00;

    LPSPI1_transfer.dataSize = 2;
    LPSPI1_transfer.configFlags = kLPSPI_MasterPcsContinuous | kLPSPI_MasterPcs0;

	in_progress = true;

	while (LPSPI_GetStatusFlags(LPSPI1) & kLPSPI_ModuleBusyFlag) {};
    assert (LPSPI_MasterTransferBlocking(LPSPI1, &LPSPI1_transfer) == kStatus_Success);

    in_progress = false;

    return LPSPI1_rxBuffer[1];
}

static void lora_init_spi()
{
    memset(&(lpspiEdmaMasterRxRegToRxDataHandle), 0, sizeof(lpspiEdmaMasterRxRegToRxDataHandle));
    memset(&(lpspiEdmaMasterTxDataToTxRegHandle), 0, sizeof(lpspiEdmaMasterTxDataToTxRegHandle));

	EDMA_CreateHandle(&lpspiEdmaMasterTxDataToTxRegHandle, DMA0, DMA0_CH0_DMA_CHANNEL);
	EDMA_CreateHandle(&lpspiEdmaMasterRxRegToRxDataHandle, DMA0, DMA0_CH1_DMA_CHANNEL);

	// LPSPI_MasterTransferCreateHandleEDMA(LPSPI1, &g_m_edma_handle, LPSPI_MasterUserCallback, NULL, &lpspiEdmaMasterRxRegToRxDataHandle, &lpspiEdmaMasterTxDataToTxRegHandle);
}

void init_radio()
{
	lora_init_spi();
	lora_init(&lora_dev);

#ifdef DEBUG
	radio_status_t status = radio_read_status();

	if (status == rsNotInitizlied) {
		PRINTF("Radio initialization failed\n");
	} else {
		PRINTF("Radio initialized\n");
	}
#endif
}


radio_status_t radio_read_status()
{
	uint8_t status = lora_read_reg8(RFM_STATUS_REG);
	if (status & 0x80) {
		status &= 0x0f;

		switch (status) {
		  case 0x0:
			return rsSleep;
		  case 0x1:
			return rsStandby;
		  default:
			return rsUnknown;
		}
	} else {
		return rsNotInitizlied;
	}
}


void register_receive_callback(radio_receive_callback_t func)
{
	callback = func;
}


void unregister_receive_callback()
{
	callback = NULL;
}

int get_last_packet_rssi()
{
	return lora_read_reg8(LORA_REG_RSSI) - 137;
}

int get_last_packet_snr()
{
	return lora_read_reg8(LORA_REG_SNR)  / 4;
}

static void radio_on_received(void *data, size_t size)
{
	PRINTF("[rssi: %d, snr %d, size %d] Lora received %s\n", get_last_packet_rssi(), get_last_packet_snr(), size, data);

	if (callback) callback(data, size);
}

void radio_receive()
{
	lora_init_receive_async(&lora_dev);
}


void pull_if_available()
{
	if (lora_is_received(&lora_dev)) {
		lora_receive(&lora_dev, radio_on_received);
	}
}

void radio_send(char *data, size_t size)
{
	lora_tx_request_t req = {
		.size = size,
		.data = (uint8_t*) data
	};

	lora_send(&lora_dev, &req);
}
