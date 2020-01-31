#include <stdio.h>
#include <cstring>
#include <unistd.h>

extern "C" {
  #include <mpsse.h>
  #include "lora_drv/lora_drv.h"
}


#define WAIT     7
#define RESET    6

// Colors (RGB565)
#define	RA8875_BLACK            0x0000 ///< Black Color
#define	RA8875_BLUE             0x001F ///< Blue Color
#define	RA8875_RED              0xF800 ///< Red Color
#define	RA8875_GREEN            0x07E0 ///< Green Color
#define RA8875_CYAN             0x07FF ///< Cyan Color
#define RA8875_MAGENTA          0xF81F ///< Magenta Color
#define RA8875_YELLOW           0xFFE0 ///< Yellow Color
#define RA8875_WHITE            0xFFFF ///< White Color

// Command/Data pins for SPI
#define RA8875_DATAWRITE        0x00 ///< See datasheet
#define RA8875_DATAREAD         0x40 ///< See datasheet
#define RA8875_CMDWRITE         0x80 ///< See datasheet
#define RA8875_CMDREAD          0xC0 ///< See datasheet

// Registers & bits
#define RA8875_PWRR             0x01 ///< See datasheet
#define RA8875_PWRR_DISPON      0x80 ///< See datasheet
#define RA8875_PWRR_DISPOFF     0x00 ///< See datasheet
#define RA8875_PWRR_SLEEP       0x02 ///< See datasheet
#define RA8875_PWRR_NORMAL      0x00 ///< See datasheet
#define RA8875_PWRR_SOFTRESET   0x01 ///< See datasheet

#define RA8875_MRWC             0x02 ///< See datasheet

#define RA8875_GPIOX            0xC7 ///< See datasheet

#define RA8875_PLLC1            0x88 ///< See datasheet
#define RA8875_PLLC1_PLLDIV2    0x80 ///< See datasheet
#define RA8875_PLLC1_PLLDIV1    0x00 ///< See datasheet

#define RA8875_PLLC2            0x89 ///< See datasheet
#define RA8875_PLLC2_DIV1       0x00 ///< See datasheet
#define RA8875_PLLC2_DIV2       0x01 ///< See datasheet
#define RA8875_PLLC2_DIV4       0x02 ///< See datasheet
#define RA8875_PLLC2_DIV8       0x03 ///< See datasheet
#define RA8875_PLLC2_DIV16      0x04 ///< See datasheet
#define RA8875_PLLC2_DIV32      0x05 ///< See datasheet
#define RA8875_PLLC2_DIV64      0x06 ///< See datasheet
#define RA8875_PLLC2_DIV128     0x07 ///< See datasheet

#define RA8875_SYSR             0x10 ///< See datasheet
#define RA8875_SYSR_8BPP        0x00 ///< See datasheet
#define RA8875_SYSR_16BPP       0x0C ///< See datasheet
#define RA8875_SYSR_MCU8        0x00 ///< See datasheet
#define RA8875_SYSR_MCU16       0x03 ///< See datasheet

#define RA8875_PCSR             0x04 ///< See datasheet
#define RA8875_PCSR_PDATR       0x00 ///< See datasheet
#define RA8875_PCSR_PDATL       0x80 ///< See datasheet
#define RA8875_PCSR_CLK         0x00 ///< See datasheet
#define RA8875_PCSR_2CLK        0x01 ///< See datasheet
#define RA8875_PCSR_4CLK        0x02 ///< See datasheet
#define RA8875_PCSR_8CLK        0x03 ///< See datasheet

#define RA8875_HDWR             0x14 ///< See datasheet

#define RA8875_HNDFTR           0x15 ///< See datasheet
#define RA8875_HNDFTR_DE_HIGH   0x00 ///< See datasheet
#define RA8875_HNDFTR_DE_LOW    0x80 ///< See datasheet

#define RA8875_HNDR             0x16 ///< See datasheet
#define RA8875_HSTR             0x17 ///< See datasheet
#define RA8875_HPWR             0x18 ///< See datasheet
#define RA8875_HPWR_LOW         0x00 ///< See datasheet
#define RA8875_HPWR_HIGH        0x80 ///< See datasheet

#define RA8875_VDHR0            0x19 ///< See datasheet
#define RA8875_VDHR1            0x1A ///< See datasheet
#define RA8875_VNDR0            0x1B ///< See datasheet
#define RA8875_VNDR1            0x1C ///< See datasheet
#define RA8875_VSTR0            0x1D ///< See datasheet
#define RA8875_VSTR1            0x1E ///< See datasheet
#define RA8875_VPWR             0x1F ///< See datasheet
#define RA8875_VPWR_LOW         0x00 ///< See datasheet
#define RA8875_VPWR_HIGH        0x80 ///< See datasheet

#define RA8875_HSAW0            0x30 ///< See datasheet
#define RA8875_HSAW1            0x31 ///< See datasheet
#define RA8875_VSAW0            0x32 ///< See datasheet
#define RA8875_VSAW1            0x33 ///< See datasheet

#define RA8875_HEAW0            0x34 ///< See datasheet
#define RA8875_HEAW1            0x35 ///< See datasheet
#define RA8875_VEAW0            0x36 ///< See datasheet
#define RA8875_VEAW1            0x37 ///< See datasheet

#define RA8875_MCLR             0x8E ///< See datasheet
#define RA8875_MCLR_START       0x80 ///< See datasheet
#define RA8875_MCLR_STOP        0x00 ///< See datasheet
#define RA8875_MCLR_READSTATUS  0x80 ///< See datasheet
#define RA8875_MCLR_FULL        0x00 ///< See datasheet
#define RA8875_MCLR_ACTIVE      0x40 ///< See datasheet

#define RA8875_DCR                    0x90 ///< See datasheet
#define RA8875_DCR_LINESQUTRI_START   0x80 ///< See datasheet
#define RA8875_DCR_LINESQUTRI_STOP    0x00 ///< See datasheet
#define RA8875_DCR_LINESQUTRI_STATUS  0x80 ///< See datasheet
#define RA8875_DCR_CIRCLE_START       0x40 ///< See datasheet
#define RA8875_DCR_CIRCLE_STATUS      0x40 ///< See datasheet
#define RA8875_DCR_CIRCLE_STOP        0x00 ///< See datasheet
#define RA8875_DCR_FILL               0x20 ///< See datasheet
#define RA8875_DCR_NOFILL             0x00 ///< See datasheet
#define RA8875_DCR_DRAWLINE           0x00 ///< See datasheet
#define RA8875_DCR_DRAWTRIANGLE       0x01 ///< See datasheet
#define RA8875_DCR_DRAWSQUARE         0x10 ///< See datasheet

#define RA8875_ELLIPSE                0xA0 ///< See datasheet
#define RA8875_ELLIPSE_STATUS         0x80 ///< See datasheet

#define RA8875_MWCR0            0x40 ///< See datasheet
#define RA8875_MWCR0_GFXMODE    0x00 ///< See datasheet
#define RA8875_MWCR0_TXTMODE    0x80 ///< See datasheet
#define RA8875_MWCR0_CURSOR     0x40 ///< See datasheet
#define RA8875_MWCR0_BLINK      0x20 ///< See datasheet

#define RA8875_MWCR0_DIRMASK    0x0C ///< Bitmask for Write Direction
#define RA8875_MWCR0_LRTD       0x00 ///< Left->Right then Top->Down
#define RA8875_MWCR0_RLTD       0x04 ///< Right->Left then Top->Down
#define RA8875_MWCR0_TDLR       0x08 ///< Top->Down then Left->Right
#define RA8875_MWCR0_DTLR       0x0C ///< Down->Top then Left->Right

#define RA8875_BTCR             0x44 ///< See datasheet
#define RA8875_CURH0            0x46 ///< See datasheet
#define RA8875_CURH1            0x47 ///< See datasheet
#define RA8875_CURV0            0x48 ///< See datasheet
#define RA8875_CURV1            0x49 ///< See datasheet

#define RA8875_P1CR             0x8A ///< See datasheet
#define RA8875_P1CR_ENABLE      0x80 ///< See datasheet
#define RA8875_P1CR_DISABLE     0x00 ///< See datasheet
#define RA8875_P1CR_CLKOUT      0x10 ///< See datasheet
#define RA8875_P1CR_PWMOUT      0x00 ///< See datasheet

#define RA8875_P1DCR            0x8B ///< See datasheet

#define RA8875_P2CR             0x8C ///< See datasheet
#define RA8875_P2CR_ENABLE      0x80 ///< See datasheet
#define RA8875_P2CR_DISABLE     0x00 ///< See datasheet
#define RA8875_P2CR_CLKOUT      0x10 ///< See datasheet
#define RA8875_P2CR_PWMOUT      0x00 ///< See datasheet

#define RA8875_P2DCR            0x8D ///< See datasheet

#define RA8875_PWM_CLK_DIV1     0x00 ///< See datasheet
#define RA8875_PWM_CLK_DIV2     0x01 ///< See datasheet
#define RA8875_PWM_CLK_DIV4     0x02 ///< See datasheet
#define RA8875_PWM_CLK_DIV8     0x03 ///< See datasheet
#define RA8875_PWM_CLK_DIV16    0x04 ///< See datasheet
#define RA8875_PWM_CLK_DIV32    0x05 ///< See datasheet
#define RA8875_PWM_CLK_DIV64    0x06 ///< See datasheet
#define RA8875_PWM_CLK_DIV128   0x07 ///< See datasheet
#define RA8875_PWM_CLK_DIV256   0x08 ///< See datasheet
#define RA8875_PWM_CLK_DIV512   0x09 ///< See datasheet
#define RA8875_PWM_CLK_DIV1024  0x0A ///< See datasheet
#define RA8875_PWM_CLK_DIV2048  0x0B ///< See datasheet
#define RA8875_PWM_CLK_DIV4096  0x0C ///< See datasheet
#define RA8875_PWM_CLK_DIV8192  0x0D ///< See datasheet
#define RA8875_PWM_CLK_DIV16384 0x0E ///< See datasheet
#define RA8875_PWM_CLK_DIV32768 0x0F ///< See datasheet

#define RA8875_TPCR0                  0x70 ///< See datasheet
#define RA8875_TPCR0_ENABLE           0x80 ///< See datasheet
#define RA8875_TPCR0_DISABLE          0x00 ///< See datasheet
#define RA8875_TPCR0_WAIT_512CLK      0x00 ///< See datasheet
#define RA8875_TPCR0_WAIT_1024CLK     0x10 ///< See datasheet
#define RA8875_TPCR0_WAIT_2048CLK     0x20 ///< See datasheet
#define RA8875_TPCR0_WAIT_4096CLK     0x30 ///< See datasheet
#define RA8875_TPCR0_WAIT_8192CLK     0x40 ///< See datasheet
#define RA8875_TPCR0_WAIT_16384CLK    0x50 ///< See datasheet
#define RA8875_TPCR0_WAIT_32768CLK    0x60 ///< See datasheet
#define RA8875_TPCR0_WAIT_65536CLK    0x70 ///< See datasheet
#define RA8875_TPCR0_WAKEENABLE       0x08 ///< See datasheet
#define RA8875_TPCR0_WAKEDISABLE      0x00 ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV1      0x00 ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV2      0x01 ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV4      0x02 ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV8      0x03 ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV16     0x04 ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV32     0x05 ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV64     0x06 ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV128    0x07 ///< See datasheet

#define RA8875_TPCR1            0x71 ///< See datasheet
#define RA8875_TPCR1_AUTO       0x00 ///< See datasheet
#define RA8875_TPCR1_MANUAL     0x40 ///< See datasheet
#define RA8875_TPCR1_VREFINT    0x00 ///< See datasheet
#define RA8875_TPCR1_VREFEXT    0x20 ///< See datasheet
#define RA8875_TPCR1_DEBOUNCE   0x04 ///< See datasheet
#define RA8875_TPCR1_NODEBOUNCE 0x00 ///< See datasheet
#define RA8875_TPCR1_IDLE       0x00 ///< See datasheet
#define RA8875_TPCR1_WAIT       0x01 ///< See datasheet
#define RA8875_TPCR1_LATCHX     0x02 ///< See datasheet
#define RA8875_TPCR1_LATCHY     0x03 ///< See datasheet

#define RA8875_TPXH             0x72 ///< See datasheet
#define RA8875_TPYH             0x73 ///< See datasheet
#define RA8875_TPXYL            0x74 ///< See datasheet

#define RA8875_INTC1            0xF0 ///< See datasheet
#define RA8875_INTC1_KEY        0x10 ///< See datasheet
#define RA8875_INTC1_DMA        0x08 ///< See datasheet
#define RA8875_INTC1_TP         0x04 ///< See datasheet
#define RA8875_INTC1_BTE        0x02 ///< See datasheet

#define RA8875_INTC2            0xF1 ///< See datasheet
#define RA8875_INTC2_KEY        0x10 ///< See datasheet
#define RA8875_INTC2_DMA        0x08 ///< See datasheet
#define RA8875_INTC2_TP         0x04 ///< See datasheet
#define RA8875_INTC2_BTE        0x02 ///< See datasheet

#define RA8875_SCROLL_BOTH      0x00 ///< See datasheet
#define RA8875_SCROLL_LAYER1    0x40 ///< See datasheet
#define RA8875_SCROLL_LAYER2    0x80 ///< See datasheet
#define RA8875_SCROLL_BUFFER    0xC0 ///< See datasheet

static mpsse_context *ctx = MPSSE(SPI0, ONE_MHZ, MSB);

void write_data(uint8_t addr, uint8_t *data, size_t size)
{
    PinLow(ctx, CS);

    char buf[2] = { (char) 0x80, (char) addr };
    Write(ctx, buf, 2);


    int i = 0;
    while (size--)
    {
        char buf_data[2] = { (char) 0x00, (char) data[i++]};
        Write(ctx, buf_data, 2);
    }

    PinHigh(ctx, CS);
}

void write(uint8_t addr, uint8_t data)
{
    PinLow(ctx, CS);

    char buf[2] = { (char) 0x80, (char) addr };
    Write(ctx, buf, 2);


    char buf_data[2] = { (char) 0x00, (char) data };
    Write(ctx, buf_data, 2);

    PinHigh(ctx, CS);
}

uint8_t read(uint8_t addr) 
{
    PinLow(ctx, CS);
    char data[2] = { (char) 0x80, (char) addr };
    Write(ctx, data, 2);

//    char data[2] = { 0x40, (char) addr };
//    uint8_t res = (uint8_t) *(Transfer(ctx, data, 1));

    char data_read[1] = { 0x40 };
    Write(ctx, data_read, 1);
    uint8_t res = (uint8_t) *(Read(ctx, 1));

    PinHigh(ctx, CS);

    return res;
}


uint8_t transfer(uint8_t addr)
{

    PinLow(ctx, CS);

    char data[1] = { (char) addr };
    uint8_t res = (uint8_t) *(Transfer(ctx, data, 1));
    
    PinHigh(ctx, CS);

    return res;
}

int main2() 
{

    PinLow(ctx, RESET);
    usleep(100000);
    PinHigh(ctx, RESET);

    printf("%x\n", read(0x00));

    write(RA8875_PLLC1, RA8875_PLLC1_PLLDIV1 + 10);
    usleep(10000);
    write(RA8875_PLLC2, RA8875_PLLC2_DIV4);

    write(0x01, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);
    usleep(20000);


      write(RA8875_SYSR, RA8875_SYSR_16BPP | RA8875_SYSR_MCU8);

  /* Timing values */
  uint8_t pixclk;
  uint8_t hsync_start;
  uint8_t hsync_pw;
  uint8_t hsync_finetune;
  uint8_t hsync_nondisp;
  uint8_t vsync_pw;
  uint16_t vsync_nondisp;
  uint16_t vsync_start;
  uint16_t _height = 272;
  uint8_t _voffset = 0;
  uint16_t _width = 480;

    pixclk          = RA8875_PCSR_PDATL | RA8875_PCSR_2CLK;
    hsync_nondisp   = 26;
    hsync_start     = 32;
    hsync_pw        = 96;
    hsync_finetune  = 0;
    vsync_nondisp   = 32;
    vsync_start     = 23;
    vsync_pw        = 2;

  write(RA8875_PCSR, pixclk);

  /* Horizontal settings registers */
  write(RA8875_HDWR, (_width / 8) - 1);                          // H width: (HDWR + 1) * 8 = 480
  write(RA8875_HNDFTR, RA8875_HNDFTR_DE_HIGH + hsync_finetune);
  write(RA8875_HNDR, (hsync_nondisp - hsync_finetune - 2)/8);    // H non-display: HNDR * 8 + HNDFTR + 2 = 10
  write(RA8875_HSTR, hsync_start/8 - 1);                         // Hsync start: (HSTR + 1)*8
  write(RA8875_HPWR, RA8875_HPWR_LOW + (hsync_pw/8 - 1));        // HSync pulse width = (HPWR+1) * 8

  /* Vertical settings registers */
  write(RA8875_VDHR0, (uint16_t)(_height - 1 + _voffset) & 0xFF);
  write(RA8875_VDHR1, (uint16_t)(_height - 1 + _voffset) >> 8);
  write(RA8875_VNDR0, vsync_nondisp-1);                          // V non-display period = VNDR + 1
  write(RA8875_VNDR1, vsync_nondisp >> 8);
  write(RA8875_VSTR0, vsync_start-1);                            // Vsync start position = VSTR + 1
  write(RA8875_VSTR1, vsync_start >> 8);
  write(RA8875_VPWR, RA8875_VPWR_LOW + vsync_pw - 1);            // Vsync pulse width = VPWR + 1

  /* Set active window X */
  write(RA8875_HSAW0, 0);                                        // horizontal start point
  write(RA8875_HSAW1, 0);
  write(RA8875_HEAW0, (uint16_t)(_width - 1) & 0xFF);            // horizontal end point
  write(RA8875_HEAW1, (uint16_t)(_width - 1) >> 8);

  /* Set active window Y */
  write(RA8875_VSAW0, 0 + _voffset);                              // vertical start point
  write(RA8875_VSAW1, 0 + _voffset);
  write(RA8875_VEAW0, (uint16_t)(_height - 1 + 0) & 0xFF); // vertical end point
  write(RA8875_VEAW1, (uint16_t)(_height - 1 + 0) >> 8);

  write(RA8875_GPIOX, 1);


  write(RA8875_P1CR, RA8875_P1CR_ENABLE | (RA8875_PWM_CLK_DIV1024 & 0xF));
  write(RA8875_P1DCR, 255);

 /* Set text mode */
  uint8_t temp = read(RA8875_MWCR0);
  temp |= RA8875_MWCR0_TXTMODE; // Set bit 7
  write(RA8875_MWCR0, temp);

  /* Select the internal (ROM) font */
  temp = read(0x21);
  temp &= ~((1<<7) | (1<<5)); // Clear bits 7 and 5
  write(0x21,temp);



  char data[] = "hello LoGiN";

    while (1)
    {
        write_data(RA8875_MRWC, (uint8_t*) &data, 12);
        usleep(100000);
    }
    return 0;
}


#define RESX    6 
#define DCX     7


#define ILI9341_TFTWIDTH 240  ///< ILI9341 max TFT width
#define ILI9341_TFTHEIGHT 320 ///< ILI9341 max TFT height

#define ILI9341_NOP 0x00     ///< No-op register
#define ILI9341_SWRESET 0x01 ///< Software reset register
#define ILI9341_RDDID 0x04   ///< Read display identification information
#define ILI9341_RDDST 0x09   ///< Read Display Status

#define ILI9341_SLPIN 0x10  ///< Enter Sleep Mode
#define ILI9341_SLPOUT 0x11 ///< Sleep Out
#define ILI9341_PTLON 0x12  ///< Partial Mode ON
#define ILI9341_NORON 0x13  ///< Normal Display Mode ON

#define ILI9341_RDMODE 0x0A     ///< Read Display Power Mode
#define ILI9341_RDMADCTL 0x0B   ///< Read Display MADCTL
#define ILI9341_RDPIXFMT 0x0C   ///< Read Display Pixel Format
#define ILI9341_RDIMGFMT 0x0D   ///< Read Display Image Format
#define ILI9341_RDSELFDIAG 0x0F ///< Read Display Self-Diagnostic Result

#define ILI9341_INVOFF 0x20   ///< Display Inversion OFF
#define ILI9341_INVON 0x21    ///< Display Inversion ON
#define ILI9341_GAMMASET 0x26 ///< Gamma Set
#define ILI9341_DISPOFF 0x28  ///< Display OFF
#define ILI9341_DISPON 0x29   ///< Display ON

#define ILI9341_CASET 0x2A ///< Column Address Set
#define ILI9341_PASET 0x2B ///< Page Address Set
#define ILI9341_RAMWR 0x2C ///< Memory Write
#define ILI9341_RAMRD 0x2E ///< Memory Read

#define ILI9341_PTLAR 0x30    ///< Partial Area
#define ILI9341_VSCRDEF 0x33  ///< Vertical Scrolling Definition
#define ILI9341_MADCTL 0x36   ///< Memory Access Control
#define ILI9341_VSCRSADD 0x37 ///< Vertical Scrolling Start Address
#define ILI9341_PIXFMT 0x3A   ///< COLMOD: Pixel Format Set

#define ILI9341_FRMCTR1                                                        \
  0xB1 ///< Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9341_FRMCTR2 0xB2 ///< Frame Rate Control (In Idle Mode/8 colors)
#define ILI9341_FRMCTR3                                                        \
  0xB3 ///< Frame Rate control (In Partial Mode/Full Colors)
#define ILI9341_INVCTR 0xB4  ///< Display Inversion Control
#define ILI9341_DFUNCTR 0xB6 ///< Display Function Control

#define ILI9341_PWCTR1 0xC0 ///< Power Control 1
#define ILI9341_PWCTR2 0xC1 ///< Power Control 2
#define ILI9341_PWCTR3 0xC2 ///< Power Control 3
#define ILI9341_PWCTR4 0xC3 ///< Power Control 4
#define ILI9341_PWCTR5 0xC4 ///< Power Control 5
#define ILI9341_VMCTR1 0xC5 ///< VCOM Control 1
#define ILI9341_VMCTR2 0xC7 ///< VCOM Control 2

#define ILI9341_RDID1 0xDA ///< Read ID 1
#define ILI9341_RDID2 0xDB ///< Read ID 2
#define ILI9341_RDID3 0xDC ///< Read ID 3
#define ILI9341_RDID4 0xDD ///< Read ID 4

#define ILI9341_GMCTRP1 0xE0 ///< Positive Gamma Correction
#define ILI9341_GMCTRN1 0xE1 ///< Negative Gamma Correction
//#define ILI9341_PWCTR6     0xFC

// Color definitions
#define ILI9341_BLACK 0x0000       ///<   0,   0,   0
#define ILI9341_NAVY 0x000F        ///<   0,   0, 123
#define ILI9341_DARKGREEN 0x03E0   ///<   0, 125,   0
#define ILI9341_DARKCYAN 0x03EF    ///<   0, 125, 123
#define ILI9341_MAROON 0x7800      ///< 123,   0,   0
#define ILI9341_PURPLE 0x780F      ///< 123,   0, 123
#define ILI9341_OLIVE 0x7BE0       ///< 123, 125,   0
#define ILI9341_LIGHTGREY 0xC618   ///< 198, 195, 198
#define ILI9341_DARKGREY 0x7BEF    ///< 123, 125, 123
#define ILI9341_BLUE 0x001F        ///<   0,   0, 255
#define ILI9341_GREEN 0x07E0       ///<   0, 255,   0
#define ILI9341_CYAN 0x07FF        ///<   0, 255, 255
#define ILI9341_RED 0xF800         ///< 255,   0,   0
#define ILI9341_MAGENTA 0xF81F     ///< 255,   0, 255
#define ILI9341_YELLOW 0xFFE0      ///< 255, 255,   0
#define ILI9341_WHITE 0xFFFF       ///< 255, 255, 255
#define ILI9341_ORANGE 0xFD20      ///< 255, 165,   0
#define ILI9341_GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define ILI9341_PINK 0xFC18        ///< 255, 130, 198

static const uint8_t initcmd[] = {
  0xEF, 3, 0x03, 0x80, 0x02,
  0xCF, 3, 0x00, 0xC1, 0x30,
  0xED, 4, 0x64, 0x03, 0x12, 0x81,
  0xE8, 3, 0x85, 0x00, 0x78,
  0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
  0xF7, 1, 0x20,
  0xEA, 2, 0x00, 0x00,
  ILI9341_PWCTR1  , 1, 0x23,             // Power control VRH[5:0]
  ILI9341_PWCTR2  , 1, 0x10,             // Power control SAP[2:0];BT[3:0]
  ILI9341_VMCTR1  , 2, 0x3e, 0x28,       // VCM control
  ILI9341_VMCTR2  , 1, 0x86,             // VCM control2
  ILI9341_MADCTL  , 1, 0x48,             // Memory Access Control
  ILI9341_VSCRSADD, 1, 0x00,             // Vertical scroll zero
  ILI9341_PIXFMT  , 1, 0x55,
  ILI9341_FRMCTR1 , 2, 0x00, 0x18,
  ILI9341_DFUNCTR , 3, 0x08, 0x82, 0x27, // Display Function Control
  0xF2, 1, 0x00,                         // 3Gamma Function Disable
  ILI9341_GAMMASET , 1, 0x01,             // Gamma curve selected
  ILI9341_GMCTRP1 , 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
    0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
  ILI9341_GMCTRN1 , 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
    0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
  ILI9341_SLPOUT  , 0x80,                // Exit Sleep
  ILI9341_DISPON  , 0x80,                // Display on
  0x00                                   // End of list
};

inline void write_reg(uint8_t addr, uint8_t data) 
{
    PinHigh(ctx, DCX);
    PinLow(ctx, CS);

    Write(ctx, (char *) &addr, 1);
    PinHigh(ctx, DCX);

    Write(ctx, (char *) &data, 1);

    PinHigh(ctx, CS);
}


void write_cmd(uint8_t addr) 
{
    PinLow(ctx, DCX);
    PinLow(ctx, CS);

    Write(ctx, (char *) &addr, 1);

    PinHigh(ctx, DCX);
    PinHigh(ctx, CS);
}

void write_cint(uint8_t addr, const uint8_t *data, size_t size) 
{
    PinLow(ctx, DCX);
    PinLow(ctx, CS);

    Write(ctx, (char *) &addr, 1);
    PinHigh(ctx, DCX);

    FastWrite(ctx, (char *) data, size);

    PinHigh(ctx, CS);
}

inline uint8_t read_reg(uint8_t addr) 
{

    PinLow(ctx, DCX);
    PinLow(ctx, CS);

    Write(ctx, (char *) &addr, 1);
    PinHigh(ctx, DCX);

    uint8_t res = (uint8_t) *Read(ctx, 1);

    PinHigh(ctx, CS);

    return res;
}

inline uint8_t read_reg16(uint8_t addr) 
{

    PinLow(ctx, DCX);
    PinLow(ctx, CS);

    WriteBits(ctx, addr, 7);
    PinHigh(ctx, DCX);
    WriteBits(ctx, addr & 1, 1);

    uint16_t res = (uint8_t) *Read(ctx, 1);

//    PinLow(ctx, SK);
//    PinHigh(ctx, SK);

    res = res << 8;
    res |= (uint8_t) *Read(ctx, 1);

    PinHigh(ctx, CS);

    return res;
}

int main()
{

    PinHigh(ctx, RESX);
    usleep(1000);
    PinLow(ctx, RESX);
    usleep(1000);
    PinHigh(ctx, RESX);

    PinHigh(ctx, CS);
    usleep(10000);


    const uint8_t *addr = initcmd;
    uint8_t cmd;
    size_t n;

    while (*addr > 0) {
        cmd = *(addr++);
        n = *(addr++);

        if (n == 0x80) {
            printf("%x\n", cmd);
            write_cmd(cmd);
            usleep(1000);
        } else {
            printf("%x %zu\n", cmd, n);
            write_cint(cmd, addr, n);
            addr += n;
        }

    }

    write_reg(0xbf, 0x03);

    write_reg(0x11, 0x80);

    usleep(1000);

    write_reg(0x29, 0x80);
    usleep(1000);

    uint8_t r, g ,b;
    while (true) 
    {
        uint8_t data[] = { r++, g++, b++};
        write_cint(0x3c, data, 3);
    }

    return 0;
}
