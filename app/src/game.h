#include <preference.h>
#include <dhemitus/window.h>
#include <dhemitus/logger.h>

typedef struct game {
    int update_called;
    int render_called;
    u64 time_passed;
} game;

void on_input(window_context *context);
void on_window(window_context *context);

void on_pad(window_context *context, int jid, b8 connected);
b8 on_update(window_context *context, void *game_state);
b8 on_render(window_context *context, void *game_state, u64 dt);
