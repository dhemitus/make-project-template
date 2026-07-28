#pragma once

#include "preference.h"

typedef struct frame_data {
    u64 start_time_ns;
    u64 last_time_ns;
    u64 current_time_ns;

    u64 delta_time_ns;
    double delta_time_s;

    f64 accumulator_s;

    float current_fps;
    u64 frame_count;
    u64 fps_timer_ns;
} frame_data;

