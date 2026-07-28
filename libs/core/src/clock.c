#include "SDL3/SDL_log.h"
#include "SDL3/SDL_timer.h"
#include "dhemitus/logger.h"
#include "preference.h"
#include "dhemitus/frame_data.h"
#include "dhemitus/clock.h"
#include "dhemitus/memory.h"
#include <stdio.h>

void clock_init(frame_data *frame){
    if(!frame) return;

    zero_memory(frame, sizeof(frame_data));

    frame->start_time_ns = SDL_GetTicksNS();
    frame->last_time_ns = frame->start_time_ns;
    frame->current_time_ns = frame->start_time_ns;
    frame->frame_count = 0;
    frame->fps_timer_ns = 0;
}

void clock_tick(frame_data *frame){
    if(!frame) return;

    frame->current_time_ns = SDL_GetTicksNS();
    frame->delta_time_ns = frame->current_time_ns - frame->last_time_ns;

    if(frame->delta_time_ns == 0){
        frame->delta_time_ns = 1;
    }

    frame->delta_time_s = (double)frame->delta_time_ns / 1000000000.0;

    frame->last_time_ns = frame->current_time_ns;

//    frame->frame_count++;
//    frame->fps_timer_ns += frame->delta_time_ns;

    frame->accumulator_s += frame->delta_time_s;
/*
    if (frame->fps_timer_ns >= 1000000000ULL) {
        // FIXED PASS: Switched completely to double-precision 64-bit float math.
        // This stops floating-point bit truncations, locking in perfectly accurate counters!
        double elapsed_seconds = (double)frame->fps_timer_ns / 1000000000.0;
        frame->current_fps = (float)((double)frame->frame_count / elapsed_seconds);
       
        #ifdef DEBUG
            // Multiplied delta_time_s by 1000 to display standard milliseconds cleanly
            LOG_DEBUG("[PERF]: Live Framerate: %.2f FPS | Frame Delta: %.4f ms", 
              (double)frame->current_fps, 
              frame->delta_time_s * 1000.0);
            printf("[PERF]: Live Framerate: %.2f FPS | Frame Delta: %.4f ms", 
              (double)frame->current_fps, 
              frame->delta_time_s * 1000.0);
            SDL_Log("[PERF]: Live Framerate: %.2f FPS | Frame Delta: %.4f ms", 
              (double)frame->current_fps, 
              frame->delta_time_s * 1000.0);
        #endif
        
        // Reset sub-counters for the next rolling performance second window
        frame->frame_count = 0;
        frame->fps_timer_ns = 0;
    }
*/
}
