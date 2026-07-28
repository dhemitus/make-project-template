#pragma once
#include "preference.h"
#include "dhemitus/renderer_type.h"

struct static_mesh_data;
struct platform_state;
struct window_context;

b8 renderer_init(const char *application_name, struct window_context *platform_state);
void renderer_destroy(void);
void renderer_on_resized(u16 width, u16 height);
b8 renderer_draw_frame(render_packet *packet);
