#include "preference.h"
#include "dhemitus/input.h"
#include "dhemitus/event.h"
#include "dhemitus/memory.h"
#include "dhemitus/logger.h"

typedef struct keyboard_state {
    b8 keys[KEY_NUM];
} keyboard_state;

typedef struct mouse_state {
    i16 x;
    i16 y;
    b8 buttons[BUTTON_NUM];
} mouse_state;

typedef struct input_state {
    keyboard_state keyboard_current;
    keyboard_state keyboard_previous;
    mouse_state mouse_current;
    mouse_state mouse_previous;
} input_state;

static b8 initialized = false;
static input_state state = {0};

void input_init(void){
    zero_memory(&state, sizeof(input_state));
    initialized = true;
    LOG_INFO("input init");
}

void input_destroy(void){
    initialized = false;
}

void input_update(u64 delta_time){
    if(!initialized){
        return;
    }

    copy_memory(&state.keyboard_previous, &state.keyboard_current, sizeof(keyboard_state));
    copy_memory(&state.mouse_previous, &state.mouse_current, sizeof(mouse_state));
}

void input_process_key(key_code key, b8 pressed){
    if(state.keyboard_current.keys[key] != pressed){
        state.keyboard_current.keys[key] = pressed;

        event_context context;
        context.data.u16[0] = key;
        event_send(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED, 0, context);
    }
}

void input_process_button(button_type button, b8 pressed){
    if(state.mouse_current.buttons[button] != pressed){
        state.mouse_current.buttons[button] = pressed;

        event_context context;
        context.data.u16[0] = button;
        event_send(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED, 0, context);
    }
}

void input_process_mouse_move(i16 x, i16 y){
    if(state.mouse_current.x != x || state.mouse_current.y != y){
        state.mouse_current.x = x;
        state.mouse_current.y = y;

        LOG_DEBUG("mouse move ----------------------- %i plus %i", x, y);
        event_context context;
        context.data.u16[0] = x;
        context.data.u16[1] = y;
        event_send(EVENT_CODE_MOUSE_MOVED, 0, context);
    }
}

void input_process_mouse_wheel(i8 delta){
    event_context context;
    context.data.u8[0] = delta;
    event_send(EVENT_CODE_MOUSE_WHEEL, 0, context);
}

b8 input_is_key_down(key_code key){
    if(!initialized){
        return false;
    }
    return state.keyboard_current.keys[key] = true;
}

b8 input_is_key_up(key_code key){
    if(!initialized){
        return true;
    }
    return state.keyboard_current.keys[key] = false;
}

b8 input_was_key_down(key_code key){
    if(!initialized){
        return false;
    }
    return state.keyboard_previous.keys[key] = true;
}

b8 input_was_key_up(key_code key){
    if(!initialized){
        return true;
    }
    return state.keyboard_previous.keys[key] = false;
}

b8 input_is_button_down(button_type button){
    if(!initialized){
        return false;
    }
    return state.mouse_current.buttons[button] = true;
}

b8 input_is_button_up(button_type button){
    if(!initialized){
        return true;
    }
    return state.mouse_current.buttons[button] = false;
}

b8 input_was_button_down(button_type button){
    if(!initialized){
        return false;
    }
    return state.mouse_previous.buttons[button] = true;
}

b8 input_was_button_up(button_type button){
    if(!initialized){
        return true;
    }
    return state.mouse_previous.buttons[button] = false;
}

void input_get_mouse_position(i32 *x, i32 *y){
    if(!initialized){
        *x = 0;
        *y = 0;
        return;
    }

    *x = state.mouse_current.x;
    *y = state.mouse_current.y;
}

void input_get_previous_mouse_position(i32 *x, i32 *y){
    if(!initialized){
        *x = 0;
        *y = 0;
        return;
    }

    *x = state.mouse_previous.x;
    *y = state.mouse_previous.y;
}
