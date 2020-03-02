#ifndef WAKA_INCLUDE_H
#define WAKA_INCLUDE_H

#include "waka_conf.h"
#include "../../middleware/lvgl/lvgl.h"
#include "waka_model.h"
#include "waka_splash.h"
#include "waka_input_msg.h"

#if defined(__cplusplus)
extern "C" {
#endif

struct _waka_message_list_screen;

typedef struct
{
    lv_obj_t* label;
    char *from;
    char *text;
    size_t text_size;
} waka_message_t;

typedef void (*input_message_cb_t)();

typedef void (*send_message_cb_t)(struct _waka_message_list_screen *model);

typedef waka_message_t* (*get_message_index_cb_t)(int idx);

typedef struct _waka_message_list_screen
{
    lv_obj_t* page;
    lv_obj_t* input;
    lv_obj_t* send_button;
    size_t message_size;
    char msg_to_send[MAX_MESSAGE_SIZE];

    int idx_first;
    int idx_last;

    get_message_index_cb_t get_message_index;
    input_message_cb_t input_message_cb;
    send_message_cb_t send_message_cb;
    
} waka_message_list_screen_t;



lv_obj_t* waka_message_list_screen(waka_message_list_screen_t *model);

waka_message_list_screen_t* waka_msg_list_screen_init(active_model_t *model);

void waka_msg_list_screen_set_text_to_send(waka_message_list_screen_t *model, const char *text);

void waka_msg_list_screen_destory(active_model_t *model);

lv_obj_t* waka_message_create(waka_message_t *msg, lv_obj_t *parent);

#if defined(__cplusplus)
}
#endif

#endif
