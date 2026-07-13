#pragma once

#include <SDL3/SDL.h>
#include "preference.h"

typedef enum key_code {
    KEY_UNKNOWN     = 0,

    KEY_ESCAPE      = 256,
    KEY_A           = 65,
    KEY_D           = 68,
    KEY_S           = 83,
    KEY_W           = 87,

    KEY_LEFT_SHIFT    = 340,
    KEY_LEFT_CONTROL  = 341,
    KEY_LEFT_ALT      = 342,
    KEY_LEFT_SUPER    = 343,
    KEY_RIGHT_SHIFT   = 344,
    KEY_RIGHT_CONTROL = 345,
    KEY_RIGHT_ALT     = 346,
    KEY_RIGHT_SUPER   = 347,
    KEY_MENU          = 348,

    KEY_NUM
} key_code;

typedef enum key_action {
    KEY_ACTION_UNDEFINED,
    KEY_ACTION_DOWN,
    KEY_ACTION_UP
} key_action;

typedef enum button_type {
    BUTTON_UNDEFINED,
    BUTTON_LEFT,
    BUTTON_MIDDLE,
    BUTTON_RIGHT,
    BUTTON_NUM
} button_type;

typedef enum button_action {
    BUTTON_ACTION_UNDEFINED,
    BUTTON_ACTION_PRESS,
    BUTTON_ACTION_RELEASE,
    BUTTON_ACTION_NUM,
} button_action;

typedef enum gamepad_button {
    GAMEPAD_BUTTON_A,
    GAMEPAD_BUTTON_B,
    GAMEPAD_BUTTON_X,
    GAMEPAD_BUTTON_Y
} gamepad_button; 

typedef enum gamepad_axis {
    GAMEPAD_AXIS_LEFT_X,
    GAMEPAD_AXIS_LEFT_Y,
    GAMEPAD_AXIS_RIGHT_X,
    GAMEPAD_AXIS_RIGHT_Y
} gamepad_axis;

typedef enum input_event_type {
    INPUT_EVENT_TYPE_INVALID,

    //input key
    INPUT_EVENT_TYPE_KEY_DOWN,
    INPUT_EVENT_TYPE_KEY_UP,
    INPUT_EVENT_TYPE_CHAR,

    //input mouse
    INPUT_EVENT_TYPE_MOUSE_DOWN,
    INPUT_EVENT_TYPE_MOUSE_UP,
    INPUT_EVENT_TYPE_MOUSE_SCROLL,
    INPUT_EVENT_TYPE_MOUSE_MOVE,

    //input window
    INPUT_EVENT_TYPE_WINDOW_ENTER,
    INPUT_EVENT_TYPE_WINDOW_LEAVE,

    INPUT_EVENT_TYPE_WINDOW_FOCUS,
    INPUT_EVENT_TYPE_WINDOW_UNFOCUS,
    INPUT_EVENT_TYPE_WINDOW_RESIZED,
    INPUT_EVENT_TYPE_WINDOW_MINIMIZED,
    INPUT_EVENT_TYPE_WINDOW_OCCLUDED,
    INPUT_EVENT_TYPE_WINDOW_RESTORED,
    INPUT_EVENT_TYPE_WINDOW_MAXIMIZED,

    //input gamepad
    INPUT_EVENT_TYPE_GAMEPAD_ADDED,
    INPUT_EVENT_TYPE_GAMEPAD_REMOVED,
    INPUT_EVENT_TYPE_GAMEPAD_UP,
    INPUT_EVENT_TYPE_GAMEPAD_DOWN,
    INPUT_EVENT_TYPE_GAMEPAD_MOVE,
    
    //input finger
    INPUT_EVENT_TYPE_FINGER_UP,
    INPUT_EVENT_TYPE_FINGER_DOWN,
    INPUT_EVENT_TYPE_FINGER_MOVE,

    //input file
    INPUT_EVENT_TYPE_DROP_FILE,
} input_event_type;


typedef struct input_event {
    input_event_type type;
    key_code key_code;
    key_action key_action;
    u32 char_code;
    button_type mouse_button;
    button_action mouse_action;
    b8 mouse_button_pressed;
    int mouse_clicks;
    float mouse_x;
    float mouse_y;
    float mouse_delta_x;
    float mouse_delta_y;
    float scroll_x;
    float scroll_y;
    int window_width;
    int window_height;
    u8 keys_down[KEY_NUM];
} input_event;

key_code sdl_key_to_keycode(SDL_Scancode key);
key_action sdl_key_to_keyaction(u32 action);
button_type sdl_button_to_buttontype(u8 button);
button_action sdl_button_to_buttonaction(u32 action);
input_event_type sdl_event_to_type(u32 type);
