#include "waka.h"

LV_IMG_DECLARE(logo);

static void splash_screen_unload_cb(lv_task_t *self)
{
    waka_splash_screen_t *model = (waka_splash_screen_t*) self->user_data;
    model->callback(model);
}

lv_obj_t* waka_splash_screen(waka_splash_screen_t *model)
{
    lv_obj_t* root =  lv_obj_create(NULL, NULL);
    lv_obj_set_style(root, lv_scr_act()->style_p);

    model->img = lv_img_create(root, NULL);
	lv_img_set_src(model->img, &logo);

    // clear splash screen after delay
    lv_task_t *clear_task = lv_task_create(splash_screen_unload_cb, model->delay, LV_TASK_PRIO_LOWEST, model);
    lv_task_once(clear_task);

    return root;
}

lv_obj_t* create_message_input_screen(input_message_screen_t *model)
{

   lv_obj_t *root = lv_obj_create(NULL, NULL);
   model->input = lv_ta_create(root, NULL);
   model->keyboard = lv_kb_create(root, NULL);

   lv_obj_set_width(model->input, lv_obj_get_width(lv_scr_act()));

   lv_ta_set_text(model->input, "");
   lv_ta_set_placeholder_text(model->input, "Message...");

   lv_kb_set_ta(model->keyboard, model->input);

   return root;
}

