#ifndef WAKA_SPLASH_INCLUDE_H
#define WAKA_SPLASH_INCLUDE_H

#include "waka_model.h"

#include "../../middleware/lvgl/lvgl.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct _waka_splash_screen waka_splash_screen_t;

/**
 * @brief callback called on splash screen is about to unload
 */
typedef void (*splash_screen_done_cb_t)(waka_splash_screen_t *model);

/**
 * @brief main structure of splash screen
 */
typedef struct _waka_splash_screen {
    lv_obj_t *img;
    uint32_t delay;
    splash_screen_done_cb_t callback;
} waka_splash_screen_t;


/*
 * @brief create waka splash screen 
 */
lv_obj_t* waka_splash_screen(waka_splash_screen_t *model);


waka_splash_screen_t* waka_init_splash_screen_model(active_model_t *model);

void waka_deinit_splash_screen_model(active_model_t *model);

#if defined(__cplusplus)
}
#endif
#endif
