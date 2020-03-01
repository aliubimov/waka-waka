#include "waka_splash.h"

#include <stdlib.h>

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

waka_splash_screen_t* waka_init_splash_screen_model(active_model_t *model) {
    model->splash_model = (waka_splash_screen_t*) malloc(sizeof(waka_splash_screen_t));
    return model->splash_model;
}

void waka_splash_screen_model_destory(active_model_t *model)
{
    free(model->splash_model);
}
