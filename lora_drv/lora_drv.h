/*
 * LORA Driver
 */

#ifndef LORA_DRV_INCLUDED
#define LORA_DRV_INCLUDED


#include "stdint.h"
#include "stddef.h"

#define LORA_REG_OP_MODE                0x01

#define LORA_OP_MODE_FSK                (0 << 7)
#define LORA_OP_MODE_LORA               (1 << 7)

#define LORA_OP_MODE_SLEEP              0x00
#define LORA_OP_MODE_STANDBY            0x01
#define LORA_OP_MODE_FSTX               0x02
#define LORA_OP_MODE_TX                 0x03
#define LORA_OP_MODE_FSRX               0x04
#define LORA_OP_MODE_RX_CONT            0x05
#define LORA_OP_MODE_RX                 0x06
#define LORA_OP_MODE_CAD                0x07

#define LORA_REG_FIFO                   0x00
#define LORA_REG_FIFO_ADDR_PTR          0x0d
#define LORA_REG_FIFO_RX_BASE_ADDR      0x0e
#define LORA_REG_FIFO_TX_BASE_ADDR      0x0f
#define LORA_REG_FIFO_RX_ADDR           0x25
#define LORA_REG_FIFO_RX_CURRENT_ADDR   0x10

#define LORA_REG_PAYLOAD_LENGTH         0x22
#define LORA_REG_RX_NB_BYTES            0x13        // Number of payload bytes of latest packet received

#define LORA_REG_IRQ_FLAGS              0x12
#define LORA_REG_IRQ_FLAGS_RX_TIMEOUT   (1 << 7)
#define LORA_REG_IRQ_FLAGS_RX_DONE      (1 << 6)
#define LORA_REG_IRQ_FLAGS_PAYLOAD_CRC  (1 << 5)
#define LORA_REG_IRQ_FLAGS_VALID_HEADER (1 << 4)
#define LORA_REG_IRQ_FLAGS_TX_DONE      (1 << 3)
#define LORA_REG_IRQ_FLAGS_CAD_DONE     (1 << 2)
#define LORA_REG_IRQ_FLAGS_CAD_DETECTED 0

typedef enum lora_result 
{
    lrSuccess,
    lrFailed,
    lrBusy,
    lrTimeout
} lora_result_t;


typedef struct lora_dev
{
    void (*write_reg8)(uint8_t reg, uint8_t data);
    void (*write_reg_dma)(uint8_t reg, uint8_t *data, size_t size);
    uint8_t (*read_reg8)(uint8_t reg);
} lora_dev_t;


typedef struct lora_tx_request 
{
    uint8_t *data;  // pointer to data
    size_t size;   // size of data to transfer
} lora_tx_request_t;



/*
 * Initialize LoRA device and switch it to STANDBY mode.
 */
lora_result_t lora_init(const lora_dev_t *dev);


/*
 * Switch trasmitter to STANDBY mode. Aborts any opration
 * that is in process
 */
lora_result_t lora_enable(const lora_dev_t *dev);

/*
 * Switch transmitter to SLEEP mode
 */
lora_result_t lora_disable(const lora_dev_t *dev);


lora_result_t lora_send(const lora_dev_t *dev, lora_tx_request_t *req);


lora_result_t lora_receive(const lora_dev_t *dev);


#endif
