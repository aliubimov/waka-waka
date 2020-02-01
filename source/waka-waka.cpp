/*
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    MIMXRT1011xxxxx_Project.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include <string>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MIMXRT1011.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
/* TODO: insert other include files here. */

#include "component/lora_drv/lora_drv.h"
/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */

void __attribute__((section (".ramfunc"))) write_lora_spi(uint8_t reg, uint8_t data) {
    LPSPI1_txBuffer[0] = reg | 0x80;
    LPSPI1_txBuffer[1] = data;
    while (LPSPI_GetStatusFlags(LPSPI1) & kLPSPI_ModuleBusyFlag) {};
    assert (LPSPI_MasterTransferBlocking(LPSPI1, &LPSPI1_transfer) == kStatus_Success);

}

uint8_t __attribute__((section (".ramfunc"))) read_lora_spi(uint8_t reg) {
    LPSPI1_txBuffer[0] = reg;
    LPSPI1_txBuffer[1] = 0xa;
    while (LPSPI_GetStatusFlags(LPSPI1) & kLPSPI_ModuleBusyFlag) {};
    assert (LPSPI_MasterTransferBlocking(LPSPI1, &LPSPI1_transfer) == kStatus_Success);

    return LPSPI1_rxBuffer[1];
}

int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    lora_dev_t dev = {
    		.write_reg8 = write_lora_spi,
			.read_reg8 = read_lora_spi
    };

    while (lora_init(&dev) != lrSuccess) {
    	PRINTF("LoRA initialization failed.\n");
    }

    std::string data = "hello LoGiN";

    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
    	lora_tx_request_t req = {
    			.data = (uint8_t*) data.c_str(),
				.size = 12
    	};

    	lora_send(&dev, &req);

    	lora_receive(&dev);
    }
    return 0 ;
}
