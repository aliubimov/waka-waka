#include "waka.h"

LV_IMG_DECLARE(logo);

static void clear_splash_screen(lv_task_t *self)
{
    waka_splash_screen_t *model = (waka_splash_screen_t*) self->user_data;
    lv_obj_del(model->img);

    model->callback(model->screen);
}

void waka_splash_screen(waka_splash_screen_t *model, lv_obj_t *scr)
{
	model->screen = scr;
    model->img = lv_img_create(model->screen, NULL);
	lv_img_set_src(model->img, &logo);

    // clear splash screen after delay
    lv_task_t *clear_task = lv_task_create(clear_splash_screen, model->delay, LV_TASK_PRIO_LOWEST, model);
    lv_task_once(clear_task);
}

void destory_splash(waka_splash_screen_t *model)
{
    lv_obj_del(model->img);
}

void create_message_input_screen(input_message_screen_t *model)
{

   model->screen = lv_obj_create(NULL, NULL);
   model->input = lv_ta_create(model->screen, NULL);
   model->keyboard = lv_kb_create(model->screen, NULL);

   lv_kb_set_ta(model->keyboard, model->input);
}


void destroy_message_input_screen(input_message_screen_t* model)
{
    lv_obj_del(model->screen);
}
