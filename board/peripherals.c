/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Peripherals v7.0
processor: MIMXRT1011xxxxx
package_id: MIMXRT1011DAE5A
mcu_data: ksdk2_0
processor_version: 7.0.1
board: MIMXRT1010-EVK
functionalGroups:
- name: BOARD_InitPeripherals
  UUID: 753cac3e-e8e5-4e48-ae69-afc827b7af98
  called_from_default_init: true
  selectedCore: core0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'system'
- type_id: 'system_54b53072540eeeb8f8e9343e71f28176'
- global_system_definitions: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'msg'
- type_id: 'msg_6e2baaf3b97dbeef01c0043275f9a0e7'
- global_messages: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "peripherals.h"

/***********************************************************************************************************************
 * BOARD_InitPeripherals functional group
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * LPSPI1 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'LPSPI1'
- type: 'lpspi'
- mode: 'transfer'
- custom_name_enabled: 'false'
- type_id: 'lpspi_6e21a1e0a09f0a012d683c4f91752db8'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'LPSPI1'
- config_sets:
  - transfer:
    - config:
      - transmitBuffer:
        - init: 'true'
      - receiveBuffer:
        - init: 'true'
      - dataSize: '2'
      - flags: 'kLPSPI_MasterPcs0 kLPSPI_MasterPcsContinuous'
      - callback:
        - name: ''
        - userData: ''
  - main:
    - mode: 'kLPSPI_Master'
    - clockSource: 'LpspiClock'
    - clockSourceFreq: 'BOARD_BootClockRUN'
    - master:
      - baudRate: '500000'
      - bitsPerFrame: '8'
      - cpol: 'kLPSPI_ClockPolarityActiveHigh'
      - cpha: 'kLPSPI_ClockPhaseFirstEdge'
      - direction: 'kLPSPI_MsbFirst'
      - pcsToSckDelayInNanoSec: '1000'
      - lastSckToPcsDelayInNanoSec: '1000'
      - betweenTransferDelayInNanoSec: '1000'
      - whichPcs: 'kLPSPI_Pcs0'
      - pcsActiveHighOrLow: 'kLPSPI_PcsActiveLow'
      - pinCfg: 'kLPSPI_SdiInSdoOut'
      - dataOutConfig: 'kLpspiDataOutRetained'
    - quick_selection: 'qs_master'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const lpspi_master_config_t LPSPI1_config = {
  .baudRate = 500000,
  .bitsPerFrame = 8,
  .cpol = kLPSPI_ClockPolarityActiveHigh,
  .cpha = kLPSPI_ClockPhaseFirstEdge,
  .direction = kLPSPI_MsbFirst,
  .pcsToSckDelayInNanoSec = 1000,
  .lastSckToPcsDelayInNanoSec = 1000,
  .betweenTransferDelayInNanoSec = 1000,
  .whichPcs = kLPSPI_Pcs0,
  .pcsActiveHighOrLow = kLPSPI_PcsActiveLow,
  .pinCfg = kLPSPI_SdiInSdoOut,
  .dataOutConfig = kLpspiDataOutRetained
};
lpspi_transfer_t LPSPI1_transfer = {
  .txData = LPSPI1_txBuffer,
  .rxData = LPSPI1_rxBuffer,
  .dataSize = 2,
  .configFlags = kLPSPI_MasterPcs0 | kLPSPI_MasterPcsContinuous
};
lpspi_master_handle_t LPSPI1_handle;
uint8_t LPSPI1_txBuffer[LPSPI1_BUFFER_SIZE];
uint8_t LPSPI1_rxBuffer[LPSPI1_BUFFER_SIZE];

void LPSPI1_init(void) {
  LPSPI_MasterInit(LPSPI1_PERIPHERAL, &LPSPI1_config, LPSPI1_CLOCK_FREQ);
  LPSPI_MasterTransferCreateHandle(LPSPI1_PERIPHERAL, &LPSPI1_handle, NULL, NULL);
}

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void BOARD_InitPeripherals(void)
{
  /* Initialize components */
  LPSPI1_init();
}

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void)
{
  BOARD_InitPeripherals();
}