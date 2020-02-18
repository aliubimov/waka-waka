#include "waka.h"

#include <stdio.h>
#include <stdlib.h>

#include "../utils.h"

#define OBJ_SPACING 5

static const char *s_send_button_tetx  = "Send";

static lv_style_t msg_in_style;

static lv_obj_t* waka_message_create(waka_message_t *msg, lv_obj_t *parent)
{
    // message container
    msg->label = lv_cont_create(parent, NULL);

    lv_cont_set_fit(msg->label, LV_FIT_TIGHT);
    lv_cont_set_layout(msg->label, LV_LAYOUT_GRID);
    lv_cont_set_style(msg->label, LV_CONT_STYLE_MAIN, &lv_style_plain);
    lv_obj_set_parent_event(msg->label, true);

    // receive time label 
    lv_obj_t *l_receive_from = lv_label_create(msg->label, NULL);
    lv_label_set_static_text(l_receive_from, "#eeaf00 2012-03-01 13:44# #ff0000 LoGiN#");
    lv_label_set_recolor(l_receive_from, true);
    lv_obj_set_parent_event(l_receive_from, true);

    // message text label
    lv_obj_t *l_txt = lv_label_create(msg->label, NULL);
    lv_label_set_static_text(l_txt, msg->text);
    lv_obj_set_parent_event(l_txt, true);


    return msg->label; 
}

static void input_text_click_cb(lv_obj_t * obj, lv_event_t event) {
    waka_message_list_screen_t *model = (waka_message_list_screen_t*) obj->user_data;

    switch(event) {
     case LV_EVENT_SHORT_CLICKED:
         if (model->input_message_cb) 
             model->input_message_cb();
         break;
    }

}

/**
 * @brief loads messages to the lv_page from model->idx_first to model_idx->last
 */
static void input_page_reload_msg(waka_message_list_screen_t* model)
{
    for (int i = model->idx_first; i < model->idx_last; ++i) {
        waka_message_create(model->get_message_index(i), model->page);
    }
}


static void input_page_reload_cb(lv_obj_t * obj, lv_event_t event) {
/*
 *    waka_message_list_screen_t *model = (waka_message_list_screen_t*) obj->user_data;
 *
 *    switch(event) {
 *     case LV_EVENT_LONG_PRESSED_REPEAT:
 *         if (lv_page_on_edge(obj, LV_PAGE_EDGE_BOTTOM)) {
 *            int new_last = (model->idx_last + 1) < model->message_size ? model->idx_last + 1 : model->message_size;
 *            int new_first = model->idx_first + (new_last - model->idx_last);
 *
 *            model->idx_first = new_first;
 *            model->idx_last = new_last;
 *
 *            printf("%d-%d\n", model->idx_first, model->idx_last);
 *
 *            lv_page_clean(obj);
 *            input_page_reload_msg(model);
 *
 *         }
 *         break;
 *    }
 */
}


lv_obj_t* waka_message_list_screen(waka_message_list_screen_t *model)
{

    lv_obj_t *root = lv_obj_create(NULL, NULL);


    // create page
    model->page = lv_page_create(root, NULL);
    lv_page_set_scrl_layout(model->page, LV_LAYOUT_COL_L);
    lv_obj_set_width(model->page, lv_obj_get_width(lv_scr_act()) - (2 * OBJ_SPACING));
    lv_obj_set_user_data(model->page, model);
    lv_obj_set_event_cb(model->page, input_page_reload_cb);


    // load messages
    input_page_reload_msg(model);

    // input text 
    model->input = lv_ta_create(root, NULL);
    lv_ta_set_one_line(model->input, true);
    lv_ta_set_cursor_type(model->input, LV_CURSOR_NONE);
    lv_ta_set_text(model->input, model->msg_to_send);
    lv_obj_set_user_data(model->input, model);
    lv_obj_set_event_cb(model->input, input_text_click_cb);


    // send button
    model->send_button = lv_btn_create(root, NULL);
    lv_obj_t *send_lbl = lv_label_create(model->send_button, NULL);
    lv_label_set_static_text(send_lbl, s_send_button_tetx);

    
    // align objects 
    lv_obj_align(model->page, root, LV_ALIGN_IN_TOP_LEFT, OBJ_SPACING, OBJ_SPACING);
    lv_obj_set_width(model->input, lv_obj_get_width_fit(lv_scr_act()) - lv_obj_get_width(model->send_button) - 3 * OBJ_SPACING);
    lv_obj_align(model->input, root, LV_ALIGN_IN_BOTTOM_LEFT, OBJ_SPACING, -OBJ_SPACING); 
    lv_obj_set_height(model->send_button, lv_obj_get_height(model->input));

    lv_obj_align(model->send_button, model->input, LV_ALIGN_OUT_RIGHT_BOTTOM, OBJ_SPACING, 0);
    lv_obj_set_height(model->page, lv_obj_get_y(model->input) - OBJ_SPACING * 3);

    return root;
}

waka_message_list_screen_t* waka_msg_list_screen_init(active_model_t *model)
{
    model->list_messages_model = (waka_message_list_screen_t*) malloc(sizeof(waka_message_list_screen_t));
    return model->list_messages_model;
}

void waka_msg_list_screen_destory(active_model_t *model)
{
    free(model->list_messages_model);
}
