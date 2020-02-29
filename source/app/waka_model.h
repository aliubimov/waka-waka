#ifndef WAKA_MODEL_INCLUDE
#define WAKA_MODEL_INCLUDE

struct _waka_splash_screen;
struct _waka_message_list_screen;
struct _waka_input_message_screen;

typedef union {
    struct _waka_splash_screen *splash_model;
    struct _waka_message_list_screen *list_messages_model;
    struct _waka_input_message_screen *create_message_model;

} active_model_t;

#endif
