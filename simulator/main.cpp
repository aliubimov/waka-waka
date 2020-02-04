#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include "lvgl/lvgl.h"

#include "lv_drivers/display/monitor.h"
#include "lv_drivers/indev/mouse.h"

#include "../source/app/user_interface.h"

static lv_disp_buf_t disp_buf1;
static lv_color_t buf1_1[320*10];
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

static void memory_monitor(lv_task_t * param)
{
    (void) param; /*Unused*/

    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    printf("used: %6d (%3d %%), frag: %3d %%, biggest free: %6d\n", (int)mon.total_size - mon.free_size,
            mon.used_pct,
            mon.frag_pct,
            (int)mon.free_biggest_size);

}

input_message_screen_t model;


static void on_keyboard_ok(lv_obj_t *kb, lv_event_t event)
{
    lv_kb_def_event_cb(kb, event);

    if (event == LV_EVENT_APPLY) 
    {
        destory_message_input_screen(&model);
    }

    printf("event %d\n", event);
}


int main() {

    lv_init();

    init_display();
    init_mouse();

    lv_task_create(memory_monitor, 3000, LV_TASK_PRIO_MID, NULL);


    lv_theme_t *th = lv_theme_material_init(0, NULL);
    lv_theme_set_current(th);

    th->style.kb.bg->text.font = &lv_font_roboto_16;




    create_message_input_screen(&model);
    lv_scr_load(model.screen);

    lv_obj_set_event_cb(model.keyboard, on_keyboard_ok);


    while(1) {
        lv_task_handler();

        usleep(5 * 1000);
        lv_tick_inc(5);
    }

    return 0;
}
