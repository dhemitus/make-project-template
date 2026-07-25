#include "SDL3/SDL_events.h"
#include "preference.h"
#include "dhemitus/event.h"
#include "dhemitus/engine.h"
#include "dhemitus/logger.h"
#include "dhemitus/memory.h"
#include "dhemitus/dynamic_array.h"
#include "dhemitus/input.h"
#include <SDL3/SDL.h>

void event_handler(engine *engine, const SDL_Event *event){
    //static float last_finger_dist = -1.0f;
    b8 pressed = KEY_UNKNOWN;
    key_code key = KEY_UNKNOWN;
    button_type button = BUTTON_UNDEFINED;
    event_context context = {0};

    switch (event->type) {
        case SDL_EVENT_QUIT:
            event_send(EVENT_CODE_APPLICATION_QUIT, 0, context);
            break;

        //key event
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            key = sdl_key_to_keycode(event->key.scancode);

            pressed = event->type == SDL_EVENT_KEY_DOWN;
            input_process_key(key, pressed);

            break;

        //focus event
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
            engine->window_context->has_mouse_focus = true;
            LOG_INFO("[System] Window mouse on focus true");
           break;
        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
            engine->window_context->has_mouse_focus = false;
            LOG_INFO("[System] Window mouse on focus false");
            break;
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            engine->window_context->has_input_focus = true;
            break;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            engine->window_context->has_input_focus = false;
            break;

        //mouse event
        case SDL_EVENT_MOUSE_MOTION:

            input_process_mouse_move(event->motion.x, event->motion.y);

            break;
        case SDL_EVENT_MOUSE_WHEEL:
            //still how to
            /*if(engine->on_input_callback){
                input_event_type type = sdl_event_to_type(event->type);

                input.type = type;
                input.scroll_x = (double)event->wheel.x;
                input.scroll_y = (double)event->wheel.y;
                engine->input_event = &input;
                engine->on_input_callback(engine);

            }*/
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            button = sdl_button_to_buttontype(event->button.button);

            pressed = event->type == SDL_EVENT_MOUSE_BUTTON_DOWN;
            if(button != BUTTON_NUM && button != BUTTON_UNDEFINED){
                input_process_button(button, pressed);
            }

            break;

        //visibility event
        case SDL_EVENT_WINDOW_RESIZED:

            context.data.u16[0] = event->window.data1;
            context.data.u16[1] = event->window.data2;
            event_send(EVENT_CODE_WINDOW_RESIZED, 0, context);
            break;
        case SDL_EVENT_WINDOW_OCCLUDED:
        case SDL_EVENT_WINDOW_MINIMIZED:
            LOG_INFO("minimize from event");
            event_send(EVENT_CODE_WINDOW_MINIMIZED, 0, context);
            break;
        case SDL_EVENT_WINDOW_RESTORED:
        case SDL_EVENT_WINDOW_MAXIMIZED:
            LOG_INFO("maximize from event");
            event_send(EVENT_CODE_WINDOW_MAXIMIZED, 0, context);
            break;
/*
        //controller event
        case SDL_EVENT_GAMEPAD_ADDED:
            if(context->on_gamepad_callback){
                context->on_gamepad_callback(context, (int)event->gdevice.which, true);
            }
            break;
        case SDL_EVENT_GAMEPAD_REMOVED:
            if(context->on_gamepad_callback){
                context->on_gamepad_callback(context, (int)event->gdevice.which, false);
            }
            break;

        case SDL_EVENT_GAMEPAD_BUTTON_UP:
        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            if(context->on_gamepad_button_callback){
                LOG_INFO("gamepad jid %d button %d with action %d", (int)event->button.which, (int)event->gbutton.button, (int)event->type);
            }
            break;
        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            if(context->on_gamepad_axis_callback){
                float normalize = (float)event->gaxis.value / 32767.0f;
                context->on_gamepad_axis_callback(context, (int)event->gaxis.which, (int)event->gaxis.axis, normalize);
            }
            break;


        case SDL_EVENT_DROP_FILE:{
            const char *file_path = event->drop.data;
            if(file_path){
                LOG_INFO("[dragdrop] File registered: %s", file_path);
                // If you need to keep this path active for later processing:
                // char* persistent_copy = SDL_strdup(file_path);
            }           
            break;
        }

        case SDL_EVENT_FINGER_UP:
            last_finger_dist = -1.0f;
            break;
        case SDL_EVENT_FINGER_DOWN:
            LOG_INFO("[Gesture] Finger ID %llu pressed at normalized coordinates: X:%.2f, Y:%.2f", event->tfinger.fingerID, event->tfinger.x, event->tfinger.y);
            break;
        case SDL_EVENT_FINGER_MOTION:
            if(context->on_gesture_pan_callback) context->on_gesture_pan_callback(context, event->tfinger.dx, event->tfinger.dy);
            if(context->on_gesture_pinch_callback){
                int fingers_count = 0;
                SDL_Finger **fingers = SDL_GetTouchFingers(event->tfinger.touchID, &fingers_count);

                if(fingers && fingers_count >= 2){

                } else {
                    last_finger_dist = -1.0f;
                    SDL_Finger *f1 = fingers[0];
                    SDL_Finger *f2 = fingers[1];

                    if(f1 && f2){
                        float current_dist = sqrtf(powf(f1->x - f2->x, 2) + powf(f1->y - f2->y, 2));

                        if(last_finger_dist > 1.0f){
                            float factor = current_dist / last_finger_dist;
                            context->on_gesture_pinch_callback(context, factor);
                        }
                        last_finger_dist = current_dist;
                    }
                }

                if(fingers){
                    SDL_free(fingers);
                }
            }

            /*if(SDL_fabs(event->tfinger.dx) > 0.01f || SDL_fabs(event->tfinger.dy) > 0.01f){
                LOG_INFO("[Gesture] Finger ID %llu dragging. Delta: dX:%.3f, dY:%.3f", event->tfinger.fingerID, event->tfinger.dx, event->tfinger.dy);
            }*/
            
            /*break;*/
    }
}

b8 window_poll_events(engine *engine, void *event){
    SDL_Event *evn = (SDL_Event *)event;

    while (SDL_PollEvent(evn)) {
        //LOG_INFO("-----------------------------------------------------------------------  is it running");
        event_handler(engine, evn);
    }
    return true;
}

typedef struct registered_event {
    void *listener;
    on_event_func callback;
} registered_event;

typedef struct event_code_entry {
    registered_event *events;
} event_code_entry;

#define MAX_MESSAGE_CODES 16384

typedef struct event_state {
    event_code_entry registered[MAX_MESSAGE_CODES];
} event_state;

static b8 initialized = false;
static event_state state;

b8 event_init(void){
    if(initialized == true){
        return  false;
    }

    initialized = false;
    zero_memory(&state, sizeof(state));
    initialized = true;

    LOG_INFO("event init");
    return true;
}

void event_destroy(void){
    for(u16 i = 0; i < MAX_MESSAGE_CODES; ++i){
        if(state.registered[i].events != 0){
            dynamic_array_destroy(state.registered[i].events);
            state.registered[i].events = 0;
        }
    }
}

b8 event_register(u16 code, void *listener, on_event_func on_event){
    if(initialized == false){
        return  false;
    }

    if(state.registered[code].events == 0){
        state.registered[code].events = dynamic_array_create(registered_event);
    }

    u64 registered_count = dynamic_array_length(state.registered[code].events);
    for(u64 i = 0; i < registered_count; ++i){
        if(state.registered[code].events[i].listener == listener){
            return false;
        }
    }

    registered_event event;
    event.listener = listener;
    event.callback = on_event;
    dynamic_array_push(state.registered[code].events, event);

    return true;
}

b8 event_unregister(u16 code, void *listener, on_event_func on_event){
    if(initialized == false){
        return  false;
    }

    if(state.registered[code].events == 0){
        return false;
    }

    u64 registered_count = dynamic_array_length(state.registered[code].events);
    for(u64 i = 0; i < registered_count; ++i){
        registered_event e = state.registered[code].events[i];
        if(e.listener == listener && e.callback == on_event){
            registered_event popped_event;
            dynamic_array_pop_at(state.registered[code].events, i, &popped_event);
            return true;
        }
    }

    return  false;
}

b8 event_send(u16 code, void *sender, event_context context){
    if(initialized == false){
        return  false;
    }

    if(state.registered[code].events == 0){
        return false;
    }

    u64 registered_count = dynamic_array_length(state.registered[code].events);
    for(u64 i = 0; i < registered_count; ++i){
        registered_event e = state.registered[code].events[i];
        if(e.callback(code, sender, e.listener, context)){
            return true;
        }
    }

    return  false;
}

