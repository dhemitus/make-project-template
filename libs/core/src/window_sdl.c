#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <math.h>
#include "dhemitus/input.h"
#include "dhemitus/window.h"
#include "dhemitus/logger.h"
#include "preference.h"

void event_handler(window_context *context, const SDL_Event *event){
    static float last_finger_dist = -1.0f;
    struct input_event input = {0};

    switch (event->type) {
        case SDL_EVENT_QUIT:
            context->is_running = false;
            break;

        //key event
        case SDL_EVENT_KEY_DOWN:
            if(event->key.key == SDLK_ESCAPE){
                context->is_running = false;
            }
        case SDL_EVENT_KEY_UP:
            if(context->on_input_callback){
                input_event_type type = sdl_event_to_type(event->type);
                key_action action = sdl_key_to_keyaction(event->type);
                key_code key = sdl_key_to_keycode(event->key.scancode);

                input.type = type;
                input.key_code = key;
                input.key_action = action;
                input.keys_down[0] = key;

                context->input_event = input;
                context->on_input_callback(context);
            }
            break;

        //focus event
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
            context->has_mouse_focus = true;
            LOG_INFO("[System] Window mouse on focus true");
           break;
        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
            context->has_mouse_focus = false;
            LOG_INFO("[System] Window mouse on focus false");
            break;
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            context->has_input_focus = true;
            break;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            context->has_input_focus = false;
            break;

        //mouse event
        case SDL_EVENT_MOUSE_MOTION:
            if(context->on_input_callback){
                input_event_type type = sdl_event_to_type(event->type);

                input.type = type;
                input.mouse_x = (double)event->motion.x;
                input.mouse_y = (double)event->motion.y;
                context->input_event = input;
                context->on_input_callback(context);
            }
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            if(context->on_input_callback){
                input_event_type type = sdl_event_to_type(event->type);

                input.type = type;
                input.scroll_x = (double)event->wheel.x;
                input.scroll_y = (double)event->wheel.y;
                context->input_event = input;
                context->on_input_callback(context);
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:

            if(context->on_input_callback){
                input_event_type type = sdl_event_to_type(event->type);
                button_action action = sdl_button_to_buttonaction(event->type);
                button_type button = sdl_button_to_buttontype(event->button.button);

                input.type = type;
                input.mouse_button = button;
                input.mouse_action = action;
                input.mouse_x = (double)event->button.x;
                input.mouse_y = (double)event->button.y;
                input.mouse_clicks = event->button.clicks;
                context->input_event = input;

                context->on_input_callback(context);
            }
            break;

        //visibility event
        case SDL_EVENT_WINDOW_RESIZED:
           if(context->on_window_callback){
                input_event_type type = sdl_event_to_type(event->type);
                input.type = type;
                input.window_width = event->window.data1;
                input.window_height = event->window.data2;
                context->input_event = input;

                context->on_window_callback(context);
            }
          
            break;
        case SDL_EVENT_WINDOW_OCCLUDED:
        case SDL_EVENT_WINDOW_MINIMIZED:
            if(context->on_window_callback){
                context->is_visible = false;
                input_event_type type = sdl_event_to_type(event->type);
                input.type = type;
                context->input_event = input;
                context->on_window_callback(context);
            }
            break;
        case SDL_EVENT_WINDOW_RESTORED:
        case SDL_EVENT_WINDOW_MAXIMIZED:
            if(context->on_window_callback){
                context->is_visible = true;
                input_event_type type = sdl_event_to_type(event->type);
                input.type = type;
                context->input_event = input;

                context->on_window_callback(context);
            }
            break;

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
            
            break;
    }
}

void is_vulkan_support(SDL_Window *window){
    (void)window;
    if(!SDL_Vulkan_LoadLibrary(NULL)){
        LOG_ERROR("----------------------------------------------------------------------------------this device not vulkan support");
        SDL_Log("----------------------------------------------------------------------------------this device not vulkan support");
        return;
    }

    u32 ex_count = 0;
    const char *const *exts = SDL_Vulkan_GetInstanceExtensions(&ex_count);

    if(exts == NULL || ex_count == 0){
        LOG_ERROR("----------------------------------------------------------------------------------fetch extension failed");
        SDL_Log("fetch extension failed");
    } else {
        LOG_INFO("----------------------------------------------------------------------------------fetch extension succeed");
        SDL_Log("----------------------------------------------------------------------------------fetch extension succeed");
    }

    SDL_Vulkan_UnloadLibrary();
}

b8 window_create(window_context *context, const window_config *config){

    // Initialize SDL3 Video subsystems
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD)) {
        LOG_FATAL("----------------------------------------------------------------------------------initiate window failed");
        return false;
    }
uint32_t flags = SDL_WINDOW_RESIZABLE;

#ifdef __ANDROID__
    flags |= SDL_WINDOW_VULKAN; // Mandated on mobile to bind the native surface
#else
    flags |= SDL_WINDOW_VULKAN; // Keep it active for macOS Desktop parity too!
#endif
    context->window = SDL_CreateWindow(
        config->title, 
        config->width, config->height, 
        flags
    );

    if (!context->window) {
        LOG_FATAL("----------------------------------------------------------------------------------create window failed");
        SDL_Quit();
        return false;
    }

    is_vulkan_support(context->window);

    context->renderer = SDL_CreateRenderer(context->window, NULL);
    if (!context->renderer) {
        LOG_FATAL("----------------------------------------------------------------------------------create renderer failed");
        SDL_DestroyWindow(context->window);
        SDL_Quit();
        return false;
    }

    return true;
}

void window_destroy(window_context *context){
    if(!context) return;

    SDL_DestroyRenderer(context->renderer);
    SDL_DestroyWindow(context->window);
    SDL_Quit();
}

b8 window_should_run(window_context *context){
    return context->is_running;
}

b8 window_poll_events(window_context *context, void *event){
    SDL_Event *evn = (SDL_Event *)event;

    while (SDL_PollEvent(evn)) {
        //LOG_INFO("----------------------------------------------------------------------- is it running");
        event_handler(context, evn);
    }
}

void window_swap_buffers(window_context *context){
    if(context->has_input_focus) {
        SDL_SetRenderDrawColor(context->renderer, 45, 55, 75, 255);
    }else {
        SDL_SetRenderDrawColor(context->renderer, 25, context->has_mouse_focus ? 35 : 25, 30, 255);
    }
    SDL_RenderClear(context->renderer);
    
    SDL_RenderPresent(context->renderer);
}

void window_set_input_callback(window_context *context, input_callback_func callback){
    context->on_input_callback = callback;
}

void window_set_window_callback(window_context *context, window_callback_func callback){
    context->on_window_callback = callback;
}

/*void window_set_key_callback(window_context *context, key_callback_func callback){
    context->on_key_callback = callback;
}

void window_set_mouse_button_callback(window_context *context, mouse_button_callback_func callback){
    context->on_mouse_click_callback = callback;
}

void window_set_mouse_position_callback(window_context *context, mouse_position_callback_func callback){
    context->on_mouse_move_callback = callback;
}
void window_set_mouse_scroll_callback(window_context *context, mouse_scroll_callback_func callback){
    context->on_mouse_scroll_callback = callback;
}

void window_set_resize_callback(window_context *context, resize_callback_func callback){
    context->on_resize_callback = callback;
}

void window_set_minimize_callback(window_context *context, minimize_callback_func callback){
    context->on_minimize_callback = callback;
}*/

void window_set_gamepad_callback(window_context *context, gamepad_callback_func callback){
    context->on_gamepad_callback = callback;
}

void window_set_gamepad_button_callback(window_context *context, gamepad_button_callback_func callback){
    context->on_gamepad_button_callback = callback;
}

void window_set_gamepad_axis_callback(window_context *context, gamepad_axis_callback_func callback){
    context->on_gamepad_axis_callback = callback;
}

void window_set_gesture_pinch_callback(window_context *context, gesture_pinch_callback_func callback){
    context->on_gesture_pinch_callback = callback;
}

void window_set_gesture_pan_callback(window_context *context, gesture_pan_callback_func callback){
    context->on_gesture_pan_callback = callback;
}
