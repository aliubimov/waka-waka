#define _DEFAULT_SOURCE

#include <SDL2/SDL.h>
#include "lvgl/lvgl.h"
#include "lv_drivers/display/monitor.h"

int main() {

    lv_init();
    monitor_init();

    static lv_disp_buf_t disp_buf1;
    static lv_color_t buf1_1[480*10];


    lv_disp_buf_init(&disp_buf1, &buf1_1, NULL, 480*10);


    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    disp_drv.buffer = &disp_buf1;
    disp_drv.flush_cb = monitor_flush;

    lv_disp_drv_register(&disp_drv);


    lv_obj_t *label = lv_label_create(lv_disp_get_scr_act(NULL), NULL);

    lv_label_set_text(label, "hi!!");

    lv_obj_t *keyboard = lv_kb_create(lv_disp_get_scr_act(NULL), NULL);


    while(1) {
        lv_task_handler();
    }

    return 0;
}
