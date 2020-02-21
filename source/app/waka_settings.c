#include "waka_settings.h"
#include "../waka_conf.h"

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        lv_calendar_date_t * date = lv_calendar_get_pressed_date(obj);
        if(date) {
            lv_calendar_set_today_date(obj, date);
        }
    }
}

lv_obj_t* waka_settings_create_screen() 
{
    lv_obj_t* root = lv_obj_create(NULL, NULL);

    lv_obj_t* calendar = lv_calendar_create(root, NULL);
    lv_obj_set_event_cb(calendar, event_handler);

    return root;
}


