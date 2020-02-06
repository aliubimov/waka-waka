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

void main_screen();

void create_message_input_screen(input_message_screen_t *model);

void destroy_message_input_screen(input_message_screen_t* model);

#if defined(__cplusplus)
}
#endif

#endif
