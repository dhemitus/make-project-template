#include "dhemitus/loop.h"
#include "preference.h"
#include <SDL3/SDL.h>

b8 engine_next_loop(engine_loop *engine, void *game_state, void *event){
    if(!engine) return false;
    
    engine->last_time = engine->current_time;
    engine->current_time = SDL_GetTicksNS();

    u64 dt = engine->current_time - engine->last_time;

    const u64 MAX_DT_NS = 100 * 1000 * 1000;
    if(dt > MAX_DT_NS){
        dt = MAX_DT_NS;
    }

    while (engine->accumulator > engine->update_time) {
        if(engine->on_event_callback){
            engine->on_event_callback(engine->window_context, event);
        }

        // here we update
        if(engine->on_update_callback){
            engine->on_update_callback(engine->window_context, game_state);
        }
        engine->accumulator -= engine->update_time;
    }

    // here we render
    if(engine->on_render_callback){
        engine->on_render_callback(engine->window_context, game_state, dt);
    }

    engine->accumulator += dt;

    return true;
}
