#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include "lvgl/lvgl.h"

#include "lv_drivers/display/monitor.h"
#include "lv_drivers/indev/mouse.h"

extern "C" {
#include <mpsse.h>
}

static lv_disp_buf_t disp_buf1;
static lv_color_t buf1_1[480*10];
static lv_indev_drv_t indev_drv;

static lv_style_t page_style;



void init_display() {
    monitor_init();

    lv_disp_buf_init(&disp_buf1, &buf1_1, NULL, 320*10);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);


    disp_drv.buffer = &disp_buf1;
    disp_drv.flush_cb = monitor_flush;

    lv_disp_drv_register(&disp_drv);
};

void init_mouse() {

    mouse_init();

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = mouse_read;

    lv_indev_drv_register(&indev_drv);
};

#define RESX    6 
#define DCX     7


static mpsse_context *ctx = MPSSE(SPI0, TEN_MHZ, MSB);

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
//  0xEF, 3, 0x03, 0x80, 0x02,
//  0xCF, 3, 0x00, 0xC1, 0x30,
//  0xED, 4, 0x64, 0x03, 0x12, 0x81,
//  0xE8, 3, 0x85, 0x00, 0x78,
//  0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
//  0xF7, 1, 0x20,
//  0xEA, 2, 0x00, 0x00,
//  ILI9341_PWCTR1  , 1, 0x23,             // Power control VRH[5:0]
//  ILI9341_PWCTR2  , 1, 0x10,             // Power control SAP[2:0];BT[3:0]
//  ILI9341_VMCTR1  , 2, 0x3e, 0x28,       // VCM control
//  ILI9341_VMCTR2  , 1, 0x86,             // VCM control2
  ILI9341_MADCTL  , 1, 0xe8,             // Memory Access Control
//  ILI9341_VSCRSADD, 2, 0x01, 0x40,              // Vertical scroll zero
  ILI9341_PIXFMT  , 1, 0x55,
  0x2b, 4, 0x00, 0x00, 0x00, 0xf0,
  0x2a, 4, 0x00, 0x00, 0x01, 0x40,
//  ILI9341_FRMCTR1 , 2, 0x00, 0x18,
//  ILI9341_DFUNCTR , 3, 0x08, 0x82, 0x27, // Display Function Control
//  0xF2, 1, 0x00,                         // 3Gamma Function Disable
//  ILI9341_GAMMASET , 1, 0x01,             // Gamma curve selected
//  ILI9341_GMCTRP1 , 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
//    0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
//  ILI9341_GMCTRN1 , 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
//    0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
// ILI9341_SLPOUT  , 0x80,                // Exit Sleep
//  ILI9341_DISPON  , 0x80,                // Display on
  0x00                                   // End of list
};

inline void write_reg(uint8_t addr, uint8_t data) 
{
    PinLow(ctx, DCX);
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

void lcd_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{

    /*Return if the area is out the screen*/
    if(area->x2 < 0) return;
    if(area->y2 < 0) return;
    if(area->x1 > LV_HOR_RES - 1) return;
    if(area->y1 > LV_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
    int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
    int32_t act_x2 = area->x2 > LV_HOR_RES - 1 ? LV_HOR_RES - 1 : area->x2;
    int32_t act_y2 = area->y2 > LV_VER_RES - 1 ? LV_VER_RES - 1 : area->y2;

    int16_t i;
    uint16_t full_w = area->x2 - area->x1 + 1;

    uint16_t act_w = act_x2 - act_x1 + 1;
   
    uint8_t buf[act_w * 3];

    uint8_t p[] = { (uint8_t)(act_y1 >> 8), (uint8_t) (act_y1 & 0x00ff), (uint8_t)(act_y2 >> 8), (uint8_t) (act_y2 & 0x00ff)};
    uint8_t c[] = { (uint8_t)(act_x1 >> 8), (uint8_t) (act_x1 & 0x00ff), (uint8_t)(act_x2 >> 8), (uint8_t) (act_x2 & 0x00ff)};

    write_cint(0x2a, (uint8_t* ) c, 4);
    write_cint(0x2b, (uint8_t* ) p, 4);

    write_cmd(0x2c);;

    for(i = act_y1; i <= act_y2; i++) {
        write_cint(0x3c, (uint8_t *) color_p, act_w * 2);
        color_p += full_w;
    }

    lv_disp_flush_ready(disp_drv);
}

void lcd_init() 
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

    write_reg(0x11, 0x80);

    usleep(1000);

    write_reg(0x29, 0x80);
    usleep(1000);

    lv_disp_buf_init(&disp_buf1, &buf1_1, NULL, 320*10);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);


    disp_drv.rotated = 1;
    disp_drv.buffer = &disp_buf1;
    disp_drv.flush_cb = lcd_flush;

    lv_disp_drv_register(&disp_drv);
}


int main() {

    lv_init();
    lcd_init();

    init_display();
    init_mouse();


    lv_theme_t *th = lv_theme_material_init(0, NULL);
    lv_theme_set_current(th);

    lv_style_copy(&page_style, th->style.bg);

    page_style.body.opa = LV_OPA_TRANSP;
    page_style.body.border.width = 2;
//    page_style.body.border.opa= LV_OPA_TRANSP;;

    page_style.text.opa = LV_OPA_COVER;

    lv_obj_t *scr = lv_obj_create(NULL, NULL);
    lv_scr_load(scr);

    lv_obj_t *page = lv_page_create(scr, NULL);
    lv_page_set_scrl_layout(page, LV_LAYOUT_COL_L);
    lv_obj_set_style(page, &page_style);
    lv_obj_set_size(page, lv_obj_get_width(scr), lv_obj_get_height(scr) - 60);
    lv_obj_align(page, scr, LV_ALIGN_IN_TOP_LEFT, 0, 0);


    lv_obj_t *input = lv_ta_create(scr, NULL);
    lv_ta_set_one_line(input, true);
    lv_ta_set_cursor_type(input, LV_CURSOR_BLOCK);
    lv_ta_set_placeholder_text(input, "Write message");
    lv_ta_set_text(input, "");

    lv_obj_set_width(input, lv_obj_get_width(scr) - 12);
    lv_obj_align(input, scr, LV_ALIGN_IN_BOTTOM_MID, 0, -2);



    for (int i = 0; i < 100; i++) 
    {
        char str[3];
        sprintf(str, "recevied msg %d",i);

        lv_obj_t *lbl = lv_label_create(page, NULL);
        lv_obj_set_width(lbl, lv_page_get_fit_width(page));

        lv_label_set_text(lbl, str);
        lv_page_focus(page, lbl, LV_ANIM_OFF);
    };


    while(1) {
        lv_task_handler();
        usleep(5 * 1000);

        lv_tick_inc(5);
    }

    return 0;
}
