#include "dhemitus/engine.h"
#include "preference.h"
#include "dhemitus/frame_data.h"
#include <SDL3/SDL.h>

b8 engine_next_loop(engine *engine, void *game_state, frame_data *frame_data){
    if(!engine) return false;
    
    SDL_Event event;

    frame_data->last_time = frame_data->current_time;
    frame_data->current_time = SDL_GetTicksNS();

    u64 dt = frame_data->current_time - frame_data->last_time;

    const u64 MAX_DT_NS = 100000000;
    if(dt > MAX_DT_NS){
        dt = MAX_DT_NS;
    }

    while (frame_data->accumulator > frame_data->update_time) {
        if(engine->on_event_callback){
            engine->on_event_callback(engine, &event);
        }

        // here we update
        if(engine->on_update_callback){
            engine->on_update_callback(engine->window_context, game_state);
        }
        frame_data->accumulator -= frame_data->update_time;
    }

    // here we render
    if(engine->on_render_callback){
        engine->on_render_callback(engine->window_context, game_state, dt);
    }

    frame_data->accumulator += dt;

    return true;
}

void window_set_input_callback(engine *engine, input_callback_func callback){
    engine->on_input_callback = callback;
}

void window_set_window_callback(engine *engine, window_callback_func callback){
    engine->on_window_callback = callback;
}

