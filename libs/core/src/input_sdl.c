#include <SDL3/SDL.h>
#include "SDL3/SDL_events.h"
#include "preference.h"
#include "dhemitus/input.h"

key_code sdl_key_to_keycode(SDL_Scancode key){
    key_code on_key = KEY_UNKNOWN;
    switch (key) {
        case SDL_SCANCODE_ESCAPE:   on_key = KEY_ESCAPE;  break;
        case SDL_SCANCODE_A:        on_key = KEY_A;       break;
        case SDL_SCANCODE_D:        on_key = KEY_D;       break;
        case SDL_SCANCODE_S:        on_key = KEY_S;       break;
        case SDL_SCANCODE_W:        on_key = KEY_W;       break;
        default:                    on_key = KEY_UNKNOWN; break;
    }
    return on_key;
}

key_action sdl_key_to_keyaction(u32 action){
    key_action on_action = KEY_ACTION_UNDEFINED;
    switch (action) {
        case SDL_EVENT_KEY_UP:  on_action = KEY_ACTION_UP; break;
        case SDL_EVENT_KEY_DOWN:  on_action = KEY_ACTION_DOWN; break;
    }

    return on_action;
}

button_type sdl_button_to_buttontype(u8 button){
    button_type on_button = BUTTON_UNDEFINED;
    switch (button) {
        case SDL_BUTTON_LEFT:       on_button = BUTTON_LEFT; break;
        case SDL_BUTTON_RIGHT:      on_button = BUTTON_RIGHT; break;
        case SDL_BUTTON_MIDDLE:     on_button = BUTTON_MIDDLE; break;
    }
    return on_button;
}

button_action sdl_button_to_buttonaction(u32 action){
    button_action on_button = BUTTON_ACTION_UNDEFINED;
    switch (action) {
        case SDL_EVENT_MOUSE_BUTTON_DOWN:   on_button = BUTTON_ACTION_PRESS; break;
        case SDL_EVENT_MOUSE_BUTTON_UP:     on_button = BUTTON_ACTION_RELEASE; break;
    }

    return on_button;
}

input_event_type sdl_event_to_type(u32 type){
    input_event_type on_type = INPUT_EVENT_TYPE_INVALID;
    switch (type) {
      case SDL_EVENT_KEY_DOWN:              on_type = INPUT_EVENT_TYPE_KEY_DOWN; break;
        case SDL_EVENT_KEY_UP:              on_type = INPUT_EVENT_TYPE_KEY_UP; break;

        case SDL_EVENT_MOUSE_MOTION:        on_type = INPUT_EVENT_TYPE_MOUSE_MOVE; break;
        case SDL_EVENT_MOUSE_WHEEL:         on_type = INPUT_EVENT_TYPE_MOUSE_SCROLL; break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:   on_type = INPUT_EVENT_TYPE_MOUSE_DOWN; break;
        case SDL_EVENT_MOUSE_BUTTON_UP:     on_type = INPUT_EVENT_TYPE_MOUSE_UP; break;
        case SDL_EVENT_WINDOW_RESIZED:      on_type = INPUT_EVENT_TYPE_WINDOW_RESIZED; break;
        case SDL_EVENT_WINDOW_OCCLUDED:     on_type = INPUT_EVENT_TYPE_WINDOW_OCCLUDED; break;
        case SDL_EVENT_WINDOW_MINIMIZED:    on_type = INPUT_EVENT_TYPE_WINDOW_MINIMIZED; break;
        case SDL_EVENT_WINDOW_RESTORED:     on_type = INPUT_EVENT_TYPE_WINDOW_RESTORED; break;
        case SDL_EVENT_WINDOW_MAXIMIZED:    on_type = INPUT_EVENT_TYPE_WINDOW_MAXIMIZED; break;
    }

    return on_type;
}
