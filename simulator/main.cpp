#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include "lvgl/lvgl.h"

#include "lv_drivers/display/monitor.h"
#include "lv_drivers/indev/mouse.h"

#include "../source/app/waka.h"

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


static lv_obj_t *screen;

static void on_keyboard_ok(lv_obj_t *kb, lv_event_t event)
{

    if (event == LV_EVENT_APPLY) 
    {
        return;
    }

    lv_kb_def_event_cb(kb, event);
}

static void switch_to_main(waka_splash_screen_t *m)
{

    lv_obj_del(screen);

    input_message_screen_t model;

    screen = create_message_input_screen(&model);
    lv_obj_set_event_cb(model.keyboard, on_keyboard_ok);

    lv_scr_load(screen);
}

static void show_input_screen() 
{
    lv_obj_del(screen);

    input_message_screen_t model;

    screen = create_message_input_screen(&model);
    lv_obj_set_event_cb(model.keyboard, on_keyboard_ok);

    lv_scr_load(screen);
}


waka_message_t msg[150];


static waka_message_t* get_message_index(int idx) 
{
    return &msg[idx];
}

int main() {

    lv_init();

    init_display();
    init_mouse();

    lv_task_create(memory_monitor, 3000, LV_TASK_PRIO_MID, NULL);


    lv_theme_t *th = lv_theme_material_init(0, &terminus);
    lv_theme_set_current(th);

    th->style.kb.bg->text.font = &lv_font_roboto_16;
    th->style.kb.btn.rel->text.font = &lv_font_roboto_16;
    th->style.kb.btn.pr->text.font = &lv_font_roboto_16;


    waka_splash_screen_t splash;
    splash.delay = 3000;
    splash.callback = switch_to_main;

    //screen = waka_splash_screen(&splash);
    
    waka_message_list_screen_t m;

    m.idx_first = 0;
    m.idx_last = 15;

    m.message_size = 150;

    m.msg_to_send = "Ready for big party ?";
    m.input_message_cb = show_input_screen;
    m.get_message_index = get_message_index;

    for (int i = 0; i < m.message_size; ++i) {
        msg[i].text = "Wonders happen";
    }
    
    screen = waka_message_list_screen(&m);
    lv_scr_load(screen);

    while(1) {
        lv_task_handler();

        usleep(5 * 1000);
        lv_tick_inc(5);
    }

    return 0;
}
