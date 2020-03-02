/* lora_drv.c
 *
 * Copyright (C) 2020 Andrii Liubimov
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include <stdio.h>
#include "lora_drv.h"

#define MULTIBIT_SET(addr, mask, value) (((addr)&((0xff)^(mask)))|((value)&(mask)))

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
    if (lora_disable(dev) != lrSuccess) 
    {
        return lrFailed;
    }

    lora_set_power(dev, 20);

    dev->write_reg8(LORA_REG_OP_MODE, LORA_OP_MODE_LORA | LORA_OP_MODE_SLEEP);

    if (lora_enable(dev) != lrSuccess)
    {
        return lrFailed;
    }

    return lrSuccess;
}


lora_result_t lora_set_power(const lora_dev_t *dev, uint8_t power) 
{

    if (power < 0 || power > 20) 
    {
        return lrFailed;
    }

    uint8_t reg_val = dev->read_reg8(LORA_REG_PA_CONFIG);

    if (power == 20)
    {
        reg_val |= LORA_PA_CONFIG_BOOST;
    } else 
    {
        reg_val &= ~LORA_PA_CONFIG_BOOST;
        MULTIBIT_SET(reg_val, 0x0f, power);
    }

    dev->write_reg8(LORA_REG_PA_CONFIG, reg_val);

    return lrSuccess;
}

lora_result_t lora_config(const lora_dev_t *dev, const lora_modem_config_t *config)
{


    uint8_t config_1 = (config->bandwidth << 4) 
        | (config->coding_rate << 1)
        | (config->implicit_header);


    uint8_t config_2 = (config->spreading_factor << 4)
        | (config->tx_continous_mode << 3)
        | (config->rx_payload_crc_on << 2) 
        | (config->symb_timeout >> 8);

    uint8_t config_3 = config->symb_timeout;


    dev->write_reg8(LORA_REG_MODEM_CONFIG_1, config_1);
    dev->write_reg8(LORA_REG_MODEM_CONFIG_2, config_2);
    dev->write_reg8(LORA_REG_SYMB_TIMEOUT_LSB, config_3);

    return lrSuccess;
}

uint8_t lora_get_irq_flags(const lora_dev_t *dev) 
{
    return dev->read_reg8(LORA_REG_IRQ_FLAGS);
}

lora_result_t lora_send(const lora_dev_t *dev, const lora_tx_request_t *req) 
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


lora_result_t lora_init_receive_async(const lora_dev_t *dev)
{
    dev->write_reg8(LORA_REG_IRQ_FLAGS, 0xff);
    dev->write_reg8(LORA_REG_OP_MODE, LORA_OP_MODE_RX);

	return lrSuccess;
}

uint8_t lora_is_received(const lora_dev_t *dev)
{
	return (lora_get_irq_flags(dev) & (LORA_REG_IRQ_FLAGS_RX_DONE | LORA_REG_IRQ_FLAGS_RX_TIMEOUT));
}

lora_result_t lora_receive(const lora_dev_t *dev, lora_receive_cb_t callback)
{

    while (! lora_is_received(dev)) { };

    if (lora_get_irq_flags(dev) & LORA_REG_IRQ_FLAGS_RX_TIMEOUT) 
    {
        return lrTimeout;
    }

    uint8_t fifo_p = dev->read_reg8(LORA_REG_FIFO_RX_CURRENT_ADDR);
    uint8_t packet_size = dev->read_reg8(LORA_REG_RX_NB_BYTES);

    dev->write_reg8(LORA_REG_FIFO_ADDR_PTR, fifo_p);

    int i = 0, rx_size = packet_size;
    char data[rx_size];

    while (rx_size)
    {
       uint8_t d = dev->read_reg8(LORA_REG_FIFO);
       data[i++] = d;

       --rx_size;
    }

    callback(&data, packet_size);

    return lrSuccess;
}
