/*
 * LORA Driver
 */

#ifndef LORA_DEV_INCLUDED
#define LORA_DRV_INCLUDED


#define LORA_REG_OP_MODE                0x01

#define LORA_OP_MODE_FSK                (0 << 7)
#define LORA_OP_MODE_LORA               (1 << 7)

#define LORA_OP_MODE_SLEEP              0x00
#define LORA_OP_MODE_STANDBY            0x01
#define LORA_OP_MODE_FSTX               0x02
#define LORA_OP_MODE_TX                 0x03
#define LORA_OP_MODE_FSRX               0x04
#define LORA_OP_MODE_RX                 0x05

#define LORA_REG_FIFO                   0x00
#define LORA_REG_FIFO_ADDR_PTR          0x0e
#define LORA_REG_FIFO_RX_BASE_ADDR      0x0e
#define LORA_REG_FIFO_TX_BASE_ADDR      0x0f
#define LORA_REG_FIFO_RX_ADDR           0x25

#define LORA_REG_PAYLOAD_LENGTH         0x22

#include "stdint.h"


typedef struct lora_dev {
    void (*write_reg8)(uint8_t reg, uint8_t data);
    uint8_t (*read_reg8)(uint8_t reg);
} lora_dev_t;


typedef struct tx_request {
    uint8_t *data;
    uint8_t size;
} tx_request_t;

/*
 * Initialize LoRA device
 */
void lora_init(const lora_dev_t *dev);


void lora_send(const lora_dev_t *dev, tx_request_t *req);

#endif
