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

AT_NONCACHEABLE_SECTION_INIT(lpspi_master_edma_handle_t g_m_edma_handle) = {0};
edma_handle_t lpspiEdmaMasterRxRegToRxDataHandle;
edma_handle_t lpspiEdmaMasterTxDataToTxRegHandle;

volatile static bool in_progress = false;

void LPSPI_MasterUserCallback(LPSPI_Type *base, lpspi_master_edma_handle_t *handle, status_t status, void *userData)
{
    in_progress = false;
}


void write_cmd(uint8_t cmd)
{
	while (in_progress) {
		__NOP();
	}

	GPIO_PinWrite(GPIO1, 16, 0);

	LPSPI1_txBuffer[0] = cmd;
    LPSPI1_transfer.dataSize = 1;
    while (LPSPI_GetStatusFlags(LPSPI1) & kLPSPI_ModuleBusyFlag) {};
    assert (LPSPI_MasterTransferBlocking(LPSPI1, &LPSPI1_transfer) == kStatus_Success);

    GPIO_PinWrite(GPIO1, 16, 1);
}

void write_data(uint8_t data)
{
	while (in_progress) {
		__NOP();
	}

	GPIO_PinWrite(GPIO1, 16, 1);

	LPSPI1_txBuffer[0] = data;
    LPSPI1_transfer.dataSize = 1;
    while (LPSPI_GetStatusFlags(LPSPI1) & kLPSPI_ModuleBusyFlag) {};
    assert (LPSPI_MasterTransferBlocking(LPSPI1, &LPSPI1_transfer) == kStatus_Success);
}

void write_data_dma(uint8_t *data, size_t size)
{
	lpspi_transfer_t transfer;

	transfer.txData = data;
	transfer.rxData = NULL;
	transfer.dataSize = size;
	transfer.configFlags = kLPSPI_MasterPcsContinuous | kLPSPI_MasterPcs0;

	while (in_progress) {
		__NOP();
	}

	in_progress = true;

	GPIO_PinWrite(GPIO1, 16, 1);
	LPSPI_MasterTransferEDMA(LPSPI1, &g_m_edma_handle, &transfer);
}

void write_reg8(uint8_t cmd, uint8_t data)
{
	while (in_progress) {
		__NOP();
	}

	GPIO_PinWrite(GPIO1, 16, 0);

	LPSPI1_txBuffer[0] = cmd;
    LPSPI1_transfer.dataSize = 1;
    while (LPSPI_GetStatusFlags(LPSPI1) & kLPSPI_ModuleBusyFlag) {};
    assert (LPSPI_MasterTransferBlocking(LPSPI1, &LPSPI1_transfer) == kStatus_Success);

	GPIO_PinWrite(GPIO1, 16, 1);

	LPSPI1_txBuffer[0] = data;
    LPSPI1_transfer.dataSize = 1;
    while (LPSPI_GetStatusFlags(LPSPI1) & kLPSPI_ModuleBusyFlag) {};
    assert (LPSPI_MasterTransferBlocking(LPSPI1, &LPSPI1_transfer) == kStatus_Success);
}

static ili9341_handle_t handle;

void init_dma() {
    memset(&(lpspiEdmaMasterRxRegToRxDataHandle), 0, sizeof(lpspiEdmaMasterRxRegToRxDataHandle));
    memset(&(lpspiEdmaMasterTxDataToTxRegHandle), 0, sizeof(lpspiEdmaMasterTxDataToTxRegHandle));

	EDMA_CreateHandle(&lpspiEdmaMasterTxDataToTxRegHandle, DMA0, DMA0_CH0_DMA_CHANNEL);
	EDMA_CreateHandle(&lpspiEdmaMasterRxRegToRxDataHandle, DMA0, DMA0_CH1_DMA_CHANNEL);

	LPSPI_MasterTransferCreateHandleEDMA(LPSPI1, &g_m_edma_handle, LPSPI_MasterUserCallback, NULL, &lpspiEdmaMasterRxRegToRxDataHandle, &lpspiEdmaMasterTxDataToTxRegHandle);
	in_progress = false;
}


void init_dcx() {
	gpio_pin_config_t dcx_config = {
			.direction = kGPIO_DigitalOutput
	};

	GPIO_PinInit(GPIO1, 16, &dcx_config);
}

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

	ili3941_soft_reset(&handle);
	ili9341_init_controller(&handle);

	ili9341_screen_on(&handle);

}

static lv_disp_buf_t disp_buf;
static lv_color_t buf_1[320*5];
static lv_color_t buf_2[320*5];
static lv_indev_drv_t indev_drv;

static lv_style_t page_style;

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



    for (int i = 0; i < 6; i++)
    {
        char str[3];
        sprintf(str, "recevied msg %d",i);

        lv_obj_t *lbl = lv_label_create(page, NULL);
        lv_obj_set_width(lbl, lv_page_get_fit_width(page));

        lv_label_set_text(lbl, str);
        lv_page_focus(page, lbl, LV_ANIM_OFF);
    };

    initialized = true;

    while (1)
    {
		lv_task_handler();
    }
}

