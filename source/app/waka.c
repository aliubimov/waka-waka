#include "waka.h"

LV_IMG_DECLARE(logo);

#define OBJ_SPACING 5

static const char *s_send_button_tetx  = "Send";

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

lv_obj_t* waka_message_list_screen(waka_message_list_screen_t *model)
{

    lv_obj_t *root = lv_obj_create(NULL, NULL);


    // create page
    model->page = lv_page_create(root, NULL);
    lv_obj_align(model->page, root, LV_ALIGN_IN_TOP_LEFT, OBJ_SPACING, OBJ_SPACING);
    lv_obj_set_width(model->page, lv_obj_get_width(lv_scr_act()) - (2 * OBJ_SPACING));


    // input text 
    model->input = lv_ta_create(root, NULL);
    lv_ta_set_one_line(model->input, true);
    lv_ta_set_cursor_type(model->input, LV_CURSOR_NONE);


    // send button
    model->send_button = lv_btn_create(root, NULL);
    lv_obj_t *send_lbl = lv_label_create(model->send_button, NULL);
    lv_label_set_static_text(send_lbl, s_send_button_tetx);

    
    // align objects 
    lv_obj_set_width(model->input, lv_obj_get_width_fit(lv_scr_act()) - lv_obj_get_width(model->send_button) - 3 * OBJ_SPACING);
    lv_obj_align(model->input, root, LV_ALIGN_IN_BOTTOM_LEFT, OBJ_SPACING, -OBJ_SPACING); 
    lv_obj_set_height(model->send_button, lv_obj_get_height(model->input));

    lv_obj_align(model->send_button, model->input, LV_ALIGN_OUT_RIGHT_BOTTOM, OBJ_SPACING, 0);
    lv_obj_set_height(model->page, lv_obj_get_y(model->input) - OBJ_SPACING * 3);

    return root;
}

