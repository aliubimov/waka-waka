#ifndef WAKA_SETTINGS_INCLUDE_H
#define WAKA_SETTINGS_INCLUDE_H

#include "../../middleware/lvgl/lvgl.h"

#if defined(__cplusplus)
extern "C" {
#endif

struct _waka_settings_screen {
    lv_obj_t* view;
} waka_settings_screen_t;

lv_obj_t* waka_settings_create_screen();


#if defined(__cplusplus)
}
#endif
#endif
