#include "lora_drv.h"


void lora_init(const lora_dev_t *dev) {

    // switch to LoRA mode
    dev->write_reg8(LORA_REG_OP_MODE, LORA_OP_MODE_SLEEP);
    dev->write_reg8(LORA_REG_OP_MODE, LORA_OP_MODE_LORA | LORA_OP_MODE_SLEEP);
    dev->write_reg8(LORA_REG_OP_MODE, LORA_OP_MODE_STANDBY);
}


void lora_send(const lora_dev_t *dev, tx_request_t *req) {
    dev->write_reg8(LORA_REG_FIFO_ADDR_PTR, 0x80);

    dev->write_reg8(LORA_REG_PAYLOAD_LENGTH, req->size);
    for (int i = 0; i < req->size; ++i) {
        dev->write_reg8(LORA_REG_FIFO, (uint8_t) req->data[i]);
    }

    dev->write_reg8(LORA_REG_OP_MODE, LORA_OP_MODE_TX);
}
