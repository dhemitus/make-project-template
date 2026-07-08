#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_main.h> // MANDATORY: This handles JNI handshakes for mobile
#include <vulkan/vulkan.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h> // Required for chdir()

#include "SDL3/SDL_log.h"
#include "core.h"
#include "preference.h"

void is_vulkan_support(SDL_Window *window){
    if(!SDL_Vulkan_LoadLibrary(NULL)){
        SDL_Log("----------------------------------------------------------------------------------this device not vulkan support");
        return;
    }

    u32 ex_count = 0;
    const char *const *exts = SDL_Vulkan_GetInstanceExtensions(&ex_count);

    if(exts == NULL || ex_count == 0){
        SDL_Log("fetch extension failed");
    } else {
        SDL_Log("----------------------------------------------------------------------------------fetch extension succeed");
    }

    SDL_Vulkan_UnloadLibrary();
}

// SDL3 automatically handles renaming this to the proper JNI signature behind the scenes
int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    // Initialize SDL3 Video subsystems
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return -1;
    }
#ifdef __APPLE__
    // Fix: Move the working directory to the actual bundle folder level, not the internal MacOS binary path
    char* base_path = SDL_GetBasePath();
    if (base_path) {
        // base_path is "VulkanApp.app/Contents/MacOS/"
        // We change to "VulkanApp.app/Contents/" by moving up one level
        chdir(base_path);
        chdir("..");
        SDL_free(base_path);
    }

    // Inject driver paths so Finder knows where Homebrew/Vulkan lives
    if (getenv("VK_ICD_FILENAMES") == NULL) {
        if (access("/usr/local/share/vulkan/icd.d/MoltenVK_icd.json", F_OK) == 0) {
            setenv("VK_ICD_FILENAMES", "/usr/local/share/vulkan/icd.d/MoltenVK_icd.json", 1);
        } else {
            setenv("VK_ICD_FILENAMES", "/opt/homebrew/share/vulkan/icd.d/MoltenVK_icd.json", 1);
        }
    }
#endif
    printf("[Engine] Initializing sub-modules...\n");
    int result = math_add(5, 10);
    printf("[Engine] Test function returned value: %d\n", result);

    // Spawn window surface framework 
    SDL_Window* window = SDL_CreateWindow(
        "Vulkan Engine Workspace", 
        1280, 720, 
        SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        SDL_Quit();
        return -1;
    }

    is_vulkan_support(window);

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
        SDL_Delay(16); // Thread breathing space (~60 FPS lock)
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
