#include "preference.h"
#include "dhemitus/event.h"
#include "dhemitus/engine.h"
#include "dhemitus/logger.h"
#include <SDL3/SDL.h>

void event_handler(engine *engine, const SDL_Event *event){
    //static float last_finger_dist = -1.0f;
    struct input_event input = {0};

    switch (event->type) {
        case SDL_EVENT_QUIT:
            engine->is_running = false;
            break;

        //key event
        case SDL_EVENT_KEY_DOWN:
            if(event->key.key == SDLK_ESCAPE){
                engine->is_running = false;
            }
        case SDL_EVENT_KEY_UP:
            if(engine->on_input_callback){
                input_event_type type = sdl_event_to_type(event->type);
                key_action action = sdl_key_to_keyaction(event->type);
                key_code key = sdl_key_to_keycode(event->key.scancode);

                input.type = type;
                input.key_code = key;
                input.key_action = action;
                input.keys_down[0] = key;

                engine->input_event = &input;
                engine->on_input_callback(engine);
            }
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
            if(engine->on_input_callback){
                input_event_type type = sdl_event_to_type(event->type);

                input.type = type;
                input.mouse_x = (double)event->motion.x;
                input.mouse_y = (double)event->motion.y;
                engine->input_event = &input;
                engine->on_input_callback(engine);
            }
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            if(engine->on_input_callback){
                input_event_type type = sdl_event_to_type(event->type);

                input.type = type;
                input.scroll_x = (double)event->wheel.x;
                input.scroll_y = (double)event->wheel.y;
                engine->input_event = &input;
                engine->on_input_callback(engine);
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:

            if(engine->on_input_callback){
                input_event_type type = sdl_event_to_type(event->type);
                button_action action = sdl_button_to_buttonaction(event->type);
                button_type button = sdl_button_to_buttontype(event->button.button);

                input.type = type;
                input.mouse_button = button;
                input.mouse_action = action;
                input.mouse_x = (double)event->button.x;
                input.mouse_y = (double)event->button.y;
                input.mouse_clicks = event->button.clicks;
                engine->input_event = &input;

                engine->on_input_callback(engine);
            }
            break;

        //visibility event
        case SDL_EVENT_WINDOW_RESIZED:
           if(engine->on_window_callback){
                input_event_type type = sdl_event_to_type(event->type);
                input.type = type;
                input.window_width = event->window.data1;
                input.window_height = event->window.data2;
                engine->input_event = &input;

                engine->on_window_callback(engine);
            }
          
            break;
        case SDL_EVENT_WINDOW_OCCLUDED:
        case SDL_EVENT_WINDOW_MINIMIZED:
            if(engine->on_window_callback){
                engine->is_visible = false;
                input_event_type type = sdl_event_to_type(event->type);
                input.type = type;
                engine->input_event = &input;
                engine->on_window_callback(engine);
            }
            break;
        case SDL_EVENT_WINDOW_RESTORED:
        case SDL_EVENT_WINDOW_MAXIMIZED:
            if(engine->on_window_callback){
                engine->is_visible = true;
                input_event_type type = sdl_event_to_type(event->type);
                input.type = type;
                engine->input_event = &input;

                engine->on_window_callback(engine);
            }
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

