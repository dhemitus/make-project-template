#include <preference.h>
#include <dhemitus/logger.h>
#include <dhemitus/window.h>
#include "game.h"

b8 check(b8 val){
    return val;
}

b8 on_update(window_context *context, void *game_state){
    game *state = (game *)game_state;
    (void)context;
    /*if(context->input_event.mouse_action == BUTTON_ACTION_PRESS){
        LOG_INFO("[Mouse] Click: Button action: %d  at X:%.1f, Y:%.1f (Count: %d)", context->input_event.mouse_action, context->input_event.mouse_x, context->input_event.mouse_y, context->input_event.mouse_clicks);
        LOG_INFO("[Mouse] move callback: Button:  at X:%.1f, Y:%.1f ", context->input_event.mouse_x, context->input_event.mouse_y);
    }*/
    if(context->input_event.type == INPUT_EVENT_TYPE_MOUSE_DOWN){
        LOG_INFO("[Mouse] Click: Button type: %d action: %d  at X:%.1f, Y:%.1f (Count: %d)", context->input_event.type, context->input_event.mouse_action, context->input_event.mouse_x, context->input_event.mouse_y, context->input_event.mouse_clicks);
    }

    state->update_called += 1;
    return true;
}

b8 on_render(window_context *context, void *game_state, u64 dt){
    game *state = (game *)game_state;
    state->render_called += 1;
    state->time_passed += dt;
    (void)context;
    if(state->time_passed > 1000 * 1000 * 1000){
//        LOG_INFO("update new fps: %.2f", (double)state->update_called);
//        LOG_INFO("render new fps: %.2f", (double)state->render_called);

        state->update_called = 0;
        state->render_called = 0;
        state->time_passed = 0;
    }

    return true;

}
