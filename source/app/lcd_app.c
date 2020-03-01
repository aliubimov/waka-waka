/* lora_app.c
 *
 * Copyright (C) 2020 Andrii Liubimov
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include "lcd_app.h"
#include "ili9341_drv.h"
#include "lora_app.h"

#include "fsl_gpio.h"
#include "fsl_edma.h"
#include "fsl_lpspi.h"
#include "fsl_lpspi_edma.h"
#include "peripherals.h"

#include "middleware/lvgl/lvgl.h"

#include "fsl_debug_console.h"

#include "trouch_drv/touch_spi.h"

#include "waka.h"

extern void write_reg8(uint8_t cmd, uint8_t data);
extern void write_data_dma(uint8_t *data, size_t size);
extern void write_data(uint8_t data);
extern void write_cmd(uint8_t cmd);
extern void write_reset(uint8_t val);
extern void init_dma();
extern void init_dcx();

extern volatile bool in_progress;

static ili9341_handle_t handle;
static lv_obj_t* screen;

static touch_reading_t touch;
static active_model_t model;

void lcd_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t *color_p)
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

    uint16_t act_w = act_x2 - act_x1 + 1;
    uint16_t act_h = act_y2 - act_y1 + 1;

    uint8_t p[] = { (uint8_t)(act_y1 >> 8), (uint8_t) (act_y1 & 0x00ff), (uint8_t)(act_y2 >> 8), (uint8_t) (act_y2 & 0x00ff)};
    uint8_t c[] = { (uint8_t)(act_x1 >> 8), (uint8_t) (act_x1 & 0x00ff), (uint8_t)(act_x2 >> 8), (uint8_t) (act_x2 & 0x00ff)};

    handle.write_cmd(0x2a);
    handle.write_data_dma(c, 4);

    handle.write_cmd(0x2b);
    handle.write_data_dma(p, 4);

    write_cmd(0x2c);

	handle.write_cmd(0x3c);
	handle.write_data_dma((uint8_t*) color_p, act_w * act_h * 2);

    lv_disp_flush_ready(disp_drv);
}


void init_lcd()
{
	init_dma();
	init_dcx();

	ili9341_init_default(&handle);

	handle.write_cmd = write_cmd;
	handle.write_data = write_data;
	handle.write_reg8 = write_reg8;
	handle.write_data_dma = write_data_dma;
	handle.write_reset = write_reset;

	ili9341_hard_reset(&handle);
	ili9341_init_controller(&handle);
	ili9341_sleep_out(&handle);

	ili9341_screen_on(&handle);
}

static lv_disp_buf_t disp_buf;
static lv_indev_drv_t indev_drv;

static lv_color_t buf_1[320*5];
static lv_color_t buf_2[320*5];

static bool initialized = false;

void log_to_uart(lv_log_level_t level, const char *file, uint32_t line, const char *msg)
{
	PRINTF("%s %s", level, msg);
}

/* PIT_IRQn interrupt handler */
void PIT_IRQHANDLER(void) {
	if (initialized) {
		lv_tick_inc(5);
	}

	PIT_ClearStatusFlags(PIT, PIT_CHANNEL_0, kPIT_TimerFlag);
}

bool touch_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data) {
	touch_update_reading(&touch);

	data->state = touch.is_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

	data->point.x = touch.x;
	data->point.y = touch.y;

	return false;
}

void register_touch() {
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_read;

    lv_indev_drv_register(&indev_drv);
}

static void on_user_input(input_message_screen_t *model) {
    PRINTF("%s\n", lv_ta_get_text(model->input));
}

static void show_input_screen()
{
    lv_obj_del_async(screen);
    waka_msg_list_screen_destory(&model);

    input_message_screen_t *screen_model = waka_msg_input_screen_init(&model);
    screen_model->on_screen_apply = on_user_input;

    screen = waka_msg_input_screen_create(screen_model);

    lv_scr_load(screen);
}

void usleep(int ms) {
	while (--ms) {
		__NOP();
	}
}

static waka_message_t msg[15];

static waka_message_t* get_message_index(int idx)
{
    return &msg[idx];
}


static void switch_to_main(waka_splash_screen_t *r)
{
	lv_obj_del_async(screen);

	// FIXME this is ugly
	static waka_message_list_screen_t model;
	model.msg_to_send = "hello :)";
	model.message_size = 15;
	model.idx_first = 0;
	model.idx_last = 14;
	model.get_message_index = get_message_index;
	model.input_message_cb = show_input_screen;

	for (int i = 0; i < model.message_size; ++i) {
		msg[i].text = "hello!";
	};

	screen = waka_message_list_screen(&model);

    lv_scr_load(screen);
}


void dump_radio_state(lv_task_t *self) {
	PRINTF("radio status: %x\n", radio_read_reg(0x01));
}

void app_run() {

	lv_init();

	lv_log_register_print_cb(log_to_uart);

	lv_disp_buf_init(&disp_buf, &buf_1, &buf_2, 320*5);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    disp_drv.rotated = 1;
    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = lcd_flush;

    lv_disp_drv_register(&disp_drv);

    register_touch();

    lv_theme_t *th = lv_theme_material_init(0, NULL);
    lv_theme_set_current(th);

    th->style.kb.bg->text.font = &lv_font_roboto_16;


//    input_message_screen_t model;
//    waka_message_input_screen_screate(&model);
//
//    lv_scr_load(model.screen);
    waka_splash_screen_t splash;
    splash.delay = 3000;
    splash.callback = switch_to_main;

    screen = waka_splash_screen(&splash);
    lv_scr_load(screen);


    lv_task_create(dump_radio_state, 1000, LV_TASK_PRIO_MID, NULL);

    initialized = true;

    while (1)
    {
		lv_task_handler();
    }
}

