#pragma once
#include "preference.h"

struct engine;

typedef struct event_context {
    union {
        i64 i64[2];
        u64 u64[2];
        f64 f64[2];

        i32 i32[4];
        u32 u32[4];
        f32 f32[4];

        i16 i16[8];
        u16 u16[8];

        i8 i8[16];
        u8 u8[16];       

        const char *s;
    } data;
} event_context;

typedef b8 (*on_event_func) (u16 code, void *sender, void *listener, event_context data);

b8 event_init(void);
void event_destroy(void);

b8 window_poll_events(struct engine *engine, void *event);
DHEMITUS_API b8 event_register(u16 code, void *listener, on_event_func on_event);
DHEMITUS_API b8 event_unregister(u16 code, void *listener, on_event_func on_event);
DHEMITUS_API b8 event_send(u16 code, void *sender, event_context context);

typedef enum event_code {
    EVENT_CODE_APPLICATION_QUIT = 0x01,

    EVENT_CODE_KEY_PRESSED = 0x02,
    EVENT_CODE_KEY_RELEASED = 0x03,

    EVENT_CODE_BUTTON_PRESSED = 0x04,
    EVENT_CODE_BUTTON_RELEASED = 0x05,
    EVENT_CODE_BUTTON_CLICKED = 0x06,

    EVENT_CODE_MOUSE_MOVED = 0x07,
    EVENT_CODE_MOUSE_WHEEL = 0x08,

    EVENT_CODE_WINDOW_RESIZED = 0x09,
    EVENT_CODE_WINDOW_MINIMIZED = 0x10,
    EVENT_CODE_WINDOW_MAXIMIZED = 0x11,

    EVENT_CODE_MOUSE_DRAGGED = 0x30,
    EVENT_CODE_MOUSE_DRAG_BEGIN = 0x31,
    EVENT_CODE_MOUSE_DRAG_END = 0x32,

    MAX_EVENT_CODE = 0xFF
} event_code;
