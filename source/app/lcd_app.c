/*
 * lcd_app.c
 *
 *  Created on: Feb 1, 2020
 *      Author: Andrii
 */


#include "lcd_app.h"
#include "ili9341_drv.h"
#include "fsl_gpio.h"
#include "fsl_edma.h"
#include "fsl_lpspi.h"
#include "fsl_lpspi_edma.h"
#include "peripherals.h"

#include "middleware/lvgl/lvgl.h"

#include "fsl_debug_console.h"

#include "waka.h"

#define GPIO_PIN_TIRQ 	28

#define TOUCH_X_MIN		420U
#define TOUCH_Y_MIN		320U
#define TOUCH_X_MAX 	3800U
#define TOUCH_Y_MAX		3800U


extern void write_reg8(uint8_t cmd, uint8_t data);
extern void write_data_dma(uint8_t *data, size_t size);
extern void write_data(uint8_t data);
extern void write_cmd(uint8_t cmd);
extern void write_reset(uint8_t val);
extern void init_dma();
extern void init_dcx();

extern volatile bool in_progress;

static ili9341_handle_t handle;

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

#define XPT2046_AVG		4

static int16_t x, y;
uint8_t avg_last;
static int16_t avg_buf_x[XPT2046_AVG];
static int16_t avg_buf_y[XPT2046_AVG];

static void xpt2046_avg(int16_t * x, int16_t * y)
{
    /*Shift out the oldest data*/
    uint8_t i;
    for(i = XPT2046_AVG - 1; i > 0 ; i--) {
        avg_buf_x[i] = avg_buf_x[i - 1];
        avg_buf_y[i] = avg_buf_y[i - 1];
    }

    /*Insert the new point*/
    avg_buf_x[0] = *x;
    avg_buf_y[0] = *y;
    if(avg_last < XPT2046_AVG) avg_last++;

    /*Sum the x and y coordinates*/
    int32_t x_sum = 0;
    int32_t y_sum = 0;
    for(i = 0; i < avg_last ; i++) {
        x_sum += avg_buf_x[i];
        y_sum += avg_buf_y[i];
    }

    /*Normalize the sums*/
    (*x) = (int32_t)x_sum / avg_last;
    (*y) = (int32_t)y_sum / avg_last;
}

bool touch_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data) {

	int state = ! GPIO_PinRead(GPIO1, GPIO_PIN_TIRQ);
	data->state = state ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

	if (state) {
		while (in_progress) {
			__NOP();
		};

		LPSPI1_txBuffer[0] = 0b10010000;
		LPSPI1_txBuffer[1] = 0x00;
		LPSPI1_txBuffer[2] = 0b11010000;
		LPSPI1_txBuffer[3] = 0x00;

		LPSPI1_transfer.dataSize = 5;
		LPSPI1_transfer.configFlags = kLPSPI_MasterPcsContinuous | kLPSPI_MasterPcs1;

		while (LPSPI_GetStatusFlags(LPSPI1) & kLPSPI_ModuleBusyFlag) {};
		assert (LPSPI_MasterTransferBlocking(LPSPI1, &LPSPI1_transfer) == kStatus_Success);

		x = ((LPSPI1_rxBuffer[1] << 8) | LPSPI1_rxBuffer[2]) >> 3;
		y = ((LPSPI1_rxBuffer[3] << 8) | LPSPI1_rxBuffer[4]) >> 3;

		if (x > TOUCH_X_MIN)
		{
			x -= TOUCH_X_MIN;
		} else {
			x = 0;
		}
		if (y > TOUCH_Y_MIN)
		{
			y -= TOUCH_Y_MIN;
		} else {
			y = 0;
		}

		x = (x * 320) / (TOUCH_X_MAX - TOUCH_X_MIN);
		y = (y * 240) / (TOUCH_Y_MAX - TOUCH_Y_MIN);

		if (x > 320) {
			x = 319;
		}

		if (y > 240) {
			y = 239;
		}

		xpt2046_avg(&x, &y);
	} else {
		avg_last = 0;
	}

	data->point.x = x;
	data->point.y = y;

	return false;
}

void init_touch() {
	gpio_pin_config_t touch_irq_config = {
			.direction = kGPIO_DigitalInput
	};

	GPIO_PinInit(GPIO1, GPIO_PIN_TIRQ, &touch_irq_config);

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_read;

    lv_indev_drv_register(&indev_drv);
}


void usleep(int ms) {
	while (--ms) {
		__NOP();
	}
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

    init_touch();

    lv_theme_t *th = lv_theme_material_init(0, NULL);
    lv_theme_set_current(th);

    th->style.kb.bg->text.font = &lv_font_roboto_16;


//    input_message_screen_t model;
//    create_message_input_screen(&model);
//
//    lv_scr_load(model.screen);

    main_screen(lv_scr_act());

    initialized = true;

    while (1)
    {
		lv_task_handler();
    }
}

