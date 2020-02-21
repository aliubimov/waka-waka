#ifndef WAKA_INPUT_MSG_INCLUDE_H
#define WAKA_INPUT_MSG_INCLUDE_H

#include "waka.h"

#include "../../middleware/lvgl/lvgl.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct _waka_input_message_screen input_message_screen_t;

typedef void (*on_screen_apply_cb)(input_message_screen_t *model);

typedef struct _waka_input_message_screen {
    lv_obj_t *input;
    lv_obj_t *keyboard;
    on_screen_apply_cb on_screen_apply;
} input_message_screen_t;


/**
 * @brief allocate memory and create model for input screen
 */
input_message_screen_t* waka_msg_input_screen_init(active_model_t *model);

/**
 * @bref dealocate objects of the model and free memory
 */
void waka_msg_input_screen_destory(active_model_t *model);

/**
 * @brief create waka input message screen
 */
lv_obj_t* waka_msg_input_screen_create(input_message_screen_t *model);


#if defined(__cplusplus)
}
#endif // __cplusplus
#endif // WAKA_INPUT_MSG_INCLUDE_H
