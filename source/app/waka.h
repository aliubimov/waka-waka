#ifndef WAKA_INCLUDE_H
#define WAKA_INCLUDE_H

#include "../../middleware/lvgl/lvgl.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
    lv_obj_t *screen;
    lv_obj_t *input;
    lv_obj_t *keyboard;
} input_message_screen_t;

typedef struct {
    lv_obj_t *screen;
    lv_obj_t *img;
    uint32_t delay;
    void (*callback)(lv_obj_t *screen);
} waka_splash_screen_t;

lv_obj_t* waka_splash_screen(waka_splash_screen_t *model);

lv_obj_t* create_message_input_screen(input_message_screen_t *model);

#if defined(__cplusplus)
}
#endif

#endif
