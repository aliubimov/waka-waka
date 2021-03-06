/* lora_app.c
 *
 * Copyright (C) 2020 Andrii Liubimov
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include "waka_conf.h"

#include <touch_drv/touch_spi.h>
#include "lcd_app.h"
#include "ili9341_drv.h"
#include "lora_app.h"

#include "fsl_gpio.h"
#include "fsl_edma.h"
#include "fsl_lpspi.h"
#include "fsl_lpspi_edma.h"
#include "peripherals.h"

#include "middleware/lvgl/lvgl.h"
#include "middleware/lv_conf.h"

#include "fsl_debug_console.h"

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

static int current_msg = 0;
static waka_message_t msg[150];

static int should_refresh = 0;

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

#if TOUCH_INVERT_X == 1
	data->point.x = LV_VER_RES_MAX - touch.x;
#else
	data->point.x = touch.x;
#endif

#if TOUCH_INVERT_X == 1
	data->point.y = LV_HOR_RES_MAX - touch.y;
#else
	data->point.y = touch.y;
#endif

	return false;
}

void register_touch(lv_indev_drv_t *drv) {
    lv_indev_drv_init(drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_read;

    lv_indev_drv_register(drv);
}


static waka_message_t* get_message_index(int idx)
{
    return &msg[idx];
}

static void on_user_input(input_message_screen_t *m);

static void show_input_screen()
{
    lv_obj_del_async(screen);
    waka_msg_list_screen_destory(&model);
    should_refresh = 0;

    input_message_screen_t *screen_model = waka_msg_input_screen_init(&model);
    screen_model->on_screen_apply = on_user_input;

    screen = waka_msg_input_screen_create(screen_model);
    lv_scr_load(screen);
}


static void on_received_msg(void* data, size_t size)
{
	const char *received_msg = (char*) data;

	waka_message_t *waka_message = &msg[current_msg++];

	waka_message->from = malloc(strlen(received_msg) + 1);
	strcpy(waka_message->from, received_msg);

	received_msg += strlen(received_msg) + 1;

	waka_message->text = malloc(strlen(received_msg) + 1);
	strcpy(waka_message->text, received_msg);

	if (should_refresh) {
		lv_obj_t *new_msg = waka_message_create(waka_message, model.list_messages_model->page);
		lv_page_focus(model.list_messages_model->page, new_msg, false);
	}
}

static void on_send_msg(waka_message_list_screen_t *m)
{
	const char *text = lv_ta_get_text(m->input);

	waka_message_t *waka_message = &msg[current_msg++];

	waka_message->from = DEVICE_NAME;
	waka_message->text = malloc(strlen(text) + 1);

	strcpy(waka_message->text, text);

	lv_obj_t* new_msg = waka_message_create(waka_message, m->page);
	lv_ta_set_text(m->input, "");
	lv_page_focus(m->page, new_msg, false);

	const size_t msg_size = strlen(waka_message->from) + strlen(waka_message->text) + 2;

	char msg_to_send[msg_size];
	memset(&msg_to_send, 0, msg_size);
	memcpy(&msg_to_send[0], waka_message->from, strlen(waka_message->from) + 1);
	memcpy(&msg_to_send[strlen(waka_message->from) + 1], waka_message->text, strlen(waka_message->text) + 1);

	radio_send(msg_to_send, msg_size);
	radio_receive();
}

static void on_user_input(input_message_screen_t *m) {
    char buf[MAX_MESSAGE_SIZE];
	snprintf(buf, MAX_MESSAGE_SIZE, lv_ta_get_text(m->input));

	lv_obj_del_async(screen);
    waka_msg_input_screen_destory(&model);

    waka_message_list_screen_t* screen_model = waka_msg_list_screen_init(&model);
    screen_model->message_size = 15;
    screen_model->idx_first = 0;
    screen_model->idx_last = current_msg;
    screen_model->get_message_index = get_message_index;
    screen_model->input_message_cb = show_input_screen;
    screen_model->send_message_cb = on_send_msg;
	screen = waka_message_list_screen(screen_model);

	waka_msg_list_screen_set_text_to_send(screen_model, buf);
    lv_scr_load(screen);
    should_refresh = 1;
}

void usleep(int ms) {
	while (--ms) {
		__NOP();
	}
}

static void switch_to_main(waka_splash_screen_t *r)
{
	lv_obj_del_async(screen);
	waka_splash_screen_model_destory(&model);

    waka_message_list_screen_t* screen_model = waka_msg_list_screen_init(&model);
    screen_model->message_size = 15;
    screen_model->idx_first = 0;
    screen_model->idx_last = current_msg;
    screen_model->get_message_index = get_message_index;
    screen_model->input_message_cb = show_input_screen;
    screen_model->send_message_cb = on_send_msg;

	screen = waka_message_list_screen(screen_model);
    lv_scr_load(screen);

    should_refresh = 1;
}


void init_lora_receive(lv_task_t *self) {
	if (radio_read_status() == rsStandby) radio_receive();
}

void check_receive(lv_task_t *self) {

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
    register_touch(&indev_drv);

    lv_theme_t *th = lv_theme_material_init(0, NULL);
    lv_theme_set_current(th);

    th->style.kb.bg->text.font = &lv_font_roboto_16;

    waka_splash_screen_t* screen_model = waka_init_splash_screen_model(&model);
    screen_model->callback = switch_to_main;
    screen_model->delay = 3000;

    screen = waka_splash_screen(screen_model);
    lv_scr_load(screen);

    if (radio_read_status() == rsStandby) radio_receive();
    register_receive_callback(on_received_msg);

    initialized = true;

    while (1)
    {
    	pull_if_available();
		lv_task_handler();
    }
}

