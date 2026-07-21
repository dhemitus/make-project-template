#include <preference.h>
#include <dhemitus/window.h>
#include <dhemitus/logger.h>
#include <dhemitus/engine.h>

typedef struct game {
    int update_called;
    int render_called;
    u64 time_passed;
} game;

void on_input(engine *engine);
void on_window(engine *engine);

void on_pad(window_context *context, int jid, b8 connected);
b8 on_update(void *game_state);
b8 on_render(void *game_state, u64 dt);
