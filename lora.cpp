#include <stdio.h>
#include <cstring>

extern "C" {
  #include <mpsse.h>
  #include "lora_drv/lora_drv.h"
}


static  mpsse_context *ctx = MPSSE(SPI0, TEN_MHZ, MSB);

void write_16(uint16_t addr, uint8_t data)
{
    char buf[3];
    memset(&buf, 0, sizeof(buf));

    buf[0] = addr | 0x80;
    buf[1] = data >> 8;
    buf[2] = data & 0x00ff;


    PinLow(ctx, CS);
    Write(ctx, (char*) &buf, 3);
    PinHigh(ctx, CS);

}

void write(uint8_t addr, uint8_t data)
{
    printf("W: %x, %x\n", addr, data);

    char buf[2];
    memset(&buf, 0, sizeof(buf));

    buf[0] = addr | 0x80;
    buf[1] = data;

    PinLow(ctx, CS);
    Write(ctx, (char*) &buf, 2);;
    PinHigh(ctx, CS);

}

uint8_t read(uint8_t addr) 
{
    PinLow(ctx, CS);
    Write(ctx, (char*) &addr, 1);

    uint8_t res = (uint8_t) *(Read(ctx, 1));
    PinHigh(ctx, CS);

    return res;
}


int main()
{

    lora_dev_t dev = {
        .write_reg8 = write,
        .read_reg8 = read
    };

    
    lora_init(&dev);

    printf("status: %x\n", read(0x01));

    const char *data = "Hello LoGiN";

    lora_tx_request_t tx = {
        .data = (uint8_t *)data,
        .size = 11
    };


//    lora_send(&dev, &tx);

    while (true) {
        lora_receive(&dev);

    }
    return 0;
}
