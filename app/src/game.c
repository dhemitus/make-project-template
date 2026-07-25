#include <preference.h>
#include <dhemitus/logger.h>
#include <dhemitus/window.h>
#include <dhemitus/input.h>
#include <dhemitus/engine.h>
#include "game.h"

void on_pad(window_context *context, int jid, b8 connected){
    (void)context;
    LOG_INFO("[Gamepad] id %d connect: %d", jid, connected);
}

b8 on_update(void *game_state){
    game *state = (game *)game_state;

    state->update_called += 1;
    return true;
}

b8 on_render(void *game_state, u64 dt){
    game *state = (game *)game_state;
    state->render_called += 1;
    state->time_passed += dt;
    if(state->time_passed > 1000 * 1000 * 1000){
       // LOG_INFO("update new fps: %.2f", (double)state->update_called);
       // LOG_INFO("render new fps: %.2f", (double)state->render_called);

        state->update_called = 0;
        state->render_called = 0;
        state->time_passed = 0;
    }

    return true;

}
