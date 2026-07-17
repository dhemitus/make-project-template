#include <preference.h>
#include <dhemitus/window.h>
#include <dhemitus/logger.h>

typedef struct game {
    int update_called;
    int render_called;
    u64 time_passed;
} game;

b8 check(b8);
b8 on_update(window_context *context, void *game_state);

b8 on_render(window_context *context, void *game_state, u64 dt);
