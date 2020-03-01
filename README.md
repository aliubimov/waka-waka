[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

WakaWaka
===

WakaWaka is wireless communation device designed for cases, where no GSM currier coverage available. It uses LoRA (Long Rnage Radio) for communication and therotically allows trasmitting messages on a distance in few miles. 

Pinout for WakaWaka device
===

Following pin out share SPI port for RFM and Touch controller. LCD uses separate FlexSPI bus.

RFM95 Pin setup
---

NSS -> J57[6]

SCK -> J57[12]

MOSI -> J57[8]

MISO -> J57[10]

3.3V -> J60[8]

GRND -> J60[14]



LCD Pin Setup
---

VCC(5V) -> J60[10] (or any 5V stripe for shield)

GRND -> J60[14] (or any ground stripe for shield)

CS -> J26[6]

REST ->  J26[2]

DCX -> J26[10]

SDI(MOSI) -> J56[2]

SCK -> J26[4]

LED -> J60[8]

SDO(MISO) -> J57[4]


Touch Pin Setup
--- 

T_CS -> J57[16]

T_SCK -> J57[12]

T_DIN(MOSI) -> J57[8]

T_DO(MISO) -> J57[10]

T_IRQ -> J57[20] (via 1 kOhm resistor)
