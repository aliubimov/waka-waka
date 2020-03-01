#include "waka_conf.h"
#include "waka_input_msg.h"
#include "utils.h"

#include <stdlib.h>

static void create_message_on_keyboard_cb(lv_obj_t *obj, lv_event_t event)
{
    lv_kb_def_event_cb(obj, event);

    if (event == LV_EVENT_APPLY) {
        input_message_screen_t *model = (input_message_screen_t*) obj->user_data;

        if (model->on_screen_apply)
            model->on_screen_apply(model);
    }
}

lv_obj_t* waka_msg_input_screen_create(input_message_screen_t *model)
{

    lv_obj_t *root = lv_obj_create(NULL, NULL);

    model->input = lv_ta_create(root, NULL);
    model->keyboard = lv_kb_create(root, NULL);

    lv_obj_set_width(model->input, lv_obj_get_width(lv_scr_act()));

    lv_ta_set_max_length(model->input, MAX_MESSAGE_SIZE - 1);
    lv_ta_set_text(model->input, "");
    lv_ta_set_placeholder_text(model->input, "Message...");

    lv_kb_set_ta(model->keyboard, model->input);
    lv_obj_set_user_data(model->keyboard, model);
    lv_obj_set_event_cb(model->keyboard, create_message_on_keyboard_cb);

    return root;
}

input_message_screen_t* waka_msg_input_screen_init(active_model_t *model)
{
    model->create_message_model = (input_message_screen_t*) malloc(sizeof(input_message_screen_t));
    model->create_message_model->on_screen_apply = NULL;

    return model->create_message_model;
}

void waka_msg_input_screen_destory(active_model_t *model)
{
    free(model->create_message_model);
}
