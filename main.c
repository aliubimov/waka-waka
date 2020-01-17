#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <unistd.h>

#include <SDL2/SDL.h>
#include "lvgl/lvgl.h"

#include "lv_drivers/display/monitor.h"
#include "lv_drivers/indev/mouse.h"

static lv_disp_buf_t disp_buf1;
static lv_color_t buf1_1[480*10];
static lv_indev_drv_t indev_drv;

void init_display() {
    monitor_init();

    lv_disp_buf_init(&disp_buf1, &buf1_1, NULL, 480*10);

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

int main() {

    lv_init();

    init_display();
    init_mouse();

    lv_obj_t *label = lv_ta_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_t *keyboard = lv_kb_create(lv_disp_get_scr_act(NULL), NULL);

    lv_kb_set_ta(keyboard, label);


    while(1) {
        lv_task_handler();
        usleep(5 * 1000);

        lv_tick_inc(5);
    }

    return 0;
}
