#include "waka.h"

LV_IMG_DECLARE(logo);

void main_screen(lv_obj_t *scr)
{
	lv_obj_t *img = lv_img_create(scr, NULL);
	lv_img_set_src(img, &logo);
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
