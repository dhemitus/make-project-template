#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <math.h>
#include "dhemitus/window.h"
#include "dhemitus/logger.h"
#include "preference.h"

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

b8 window_create(window_context *context, i16 width, i16 height, const char *title){

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
        title, 
        width, height, 
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

void window_swap_buffers(window_context *context){
    if(context->has_input_focus) {
        SDL_SetRenderDrawColor(context->renderer, 45, 55, 75, 255);
    }else {
        SDL_SetRenderDrawColor(context->renderer, 25, context->has_mouse_focus ? 35 : 25, 30, 255);
    }
    SDL_RenderClear(context->renderer);
    
    SDL_RenderPresent(context->renderer);
}

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
