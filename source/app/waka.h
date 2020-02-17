#ifndef WAKA_INCLUDE_H
#define WAKA_INCLUDE_H

#include "../../middleware/lvgl/lvgl.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
    lv_obj_t *input;
    lv_obj_t *keyboard;
} input_message_screen_t;

typedef struct _waka_splash_screen_t waka_splash_screen_t;

/**
 * @brief callback called on splash screen is about to unload
 */
typedef void (*splash_screen_done_cb_t)(waka_splash_screen_t *model);

/**
 * @brief main structure of splash screen
 */
typedef struct _waka_splash_screen_t {
    lv_obj_t *img;
    uint32_t delay;
    splash_screen_done_cb_t callback;
} waka_splash_screen_t;


typedef struct 
{
    lv_obj_t* label;
    const char *text;
    size_t text_size;
} waka_message_t;

typedef void (*input_message_cb_t)();

typedef waka_message_t* (*get_message_index_cb_t)(int idx);

typedef struct
{
    lv_obj_t* page;
    lv_obj_t* input;
    lv_obj_t* send_button;
    size_t message_size;
    char * msg_to_send;

    int idx_first;
    int idx_last;

    get_message_index_cb_t get_message_index;
    input_message_cb_t input_message_cb;
    
} waka_message_list_screen_t;

lv_obj_t* waka_splash_screen(waka_splash_screen_t *model);

lv_obj_t* waka_message_list_screen(waka_message_list_screen_t *model);

lv_obj_t* create_message_input_screen(input_message_screen_t *model);

#if defined(__cplusplus)
}
#endif

#endif
