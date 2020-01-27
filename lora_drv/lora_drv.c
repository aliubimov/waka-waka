#include "lora_drv.h"



lora_result_t lora_enable(const lora_dev_t *dev)
{

    dev->write_reg8(LORA_REG_OP_MODE, LORA_OP_MODE_STANDBY);
    return lrSuccess;
}

lora_result_t lora_disable(const lora_dev_t *dev)
{

    dev->write_reg8(LORA_REG_OP_MODE, LORA_OP_MODE_SLEEP);;
    return lrSuccess;
}

lora_result_t lora_init(const lora_dev_t *dev) 
{

    // switch to LoRA mode
    lora_disable(dev);
    dev->write_reg8(LORA_REG_OP_MODE, LORA_OP_MODE_LORA | LORA_OP_MODE_SLEEP);
    lora_enable(dev);

    return lrSuccess;
}

uint8_t lora_get_irq_flags(const lora_dev_t *dev) 
{
    return dev->read_reg8(LORA_REG_IRQ_FLAGS);
}

lora_result_t lora_send(const lora_dev_t *dev, lora_tx_request_t *req) 
{
    uint8_t base_addr = dev->read_reg8(LORA_REG_FIFO_TX_BASE_ADDR);
    dev->write_reg8(LORA_REG_FIFO_ADDR_PTR, base_addr);

    dev->write_reg8(LORA_REG_PAYLOAD_LENGTH, req->size);

    for (size_t i = 0; i < req->size; ++i) {
        dev->write_reg8(LORA_REG_FIFO, (uint8_t) req->data[i]);
    }

    dev->write_reg8(LORA_REG_OP_MODE, LORA_OP_MODE_TX);


    while (! (lora_get_irq_flags(dev) & LORA_REG_IRQ_FLAGS_TX_DONE)) { };

    return lrSuccess;
}


