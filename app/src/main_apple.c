#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h> // Required for chdir()

#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>

#include "core.h"

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    // 1. CRITICAL: Initialize SDL3 Video FIRST. 
    // This safely sets up the application container so base paths don't crash.
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return -1;
    }

    // 2. Map Working Directory Absolutely to avoid "/" path sandboxing issues
    // 4. Run your internal structural engines tracking checks
    printf("[Engine] Initializing sub-modules...\n");
    int result = math_add(5, 10);
    printf("[Engine] Test function returned value: %d\n", result);

    // 5. Spawn Window Surface Frame
    SDL_Window* window = SDL_CreateWindow(
        "Vulkan Engine Workspace", 
        1280, 720, 
        SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        SDL_Quit();
        return -1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    bool is_running = true;
    SDL_Event event;

    while (is_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                is_running = false;
            }
        }
        // --- CLEAN CANVAS RENDERING LOOP ---
        // 1. Set the background color to a nice dark gray/blue
        SDL_SetRenderDrawColor(renderer, 30, 35, 45, 255);
        
        // 2. Clear the screen with that color
        SDL_RenderClear(renderer);
        
        // 3. Present the frame onto your phone screen
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
