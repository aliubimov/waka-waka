#include <stdio.h>
#include <cstring>
#include <unistd.h>

#include <string>

extern "C" {
  #include <mpsse.h>
  #include "lora_drv/lora_drv.h"
}


static mpsse_context *ctx = MPSSE(SPI0, TEN_MHZ, MSB);


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
//    printf("W: %x, %x\n", addr, data);

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


static lora_dev_t dev = {
    .write_reg8 = write,
    .read_reg8 = read
};

void lora_receive_cb(void *data, size_t size) 
{

    char *msg = (char*)data;
    printf(">> Incoming!\n");
    printf("\tfrom: %s\n", msg);
    msg += strlen(msg) + 1;
    printf("\tmsg: %s\n", msg);


    using namespace std::literals::string_literals;

    static int i = 0;
    auto smsg = "LoGiN\0Hello"s + std::to_string(i++) + "\0"s;

    const char *sdata = smsg.c_str();

    lora_tx_request_t tx = {
        .data = (uint8_t *)sdata,
        .size = 17
    };

    lora_enable(&dev);
    lora_send(&dev, &tx);
    printf("Sent %d\n", i);

    lora_init_receive_async(&dev);
}

int main()
{


    
    lora_init(&dev);

    printf("status: %x\n", read(0x01));

    lora_init_receive_async(&dev);

    printf("status: %x\n", read(0x01));

    while (true) {

        if (lora_is_received(&dev)) {
            lora_receive(&dev, lora_receive_cb);
        }

        if (read(0x01) == 0x81) {
            lora_init_receive_async(&dev);
        }
    }

    return 0;
}
