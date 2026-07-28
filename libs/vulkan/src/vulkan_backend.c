#include "dhemitus/vulkan_backend.h"
#include "SDL3/SDL_vulkan.h"
#include "dhemitus/logger.h"
#include "dhemitus/vulkan_types.h"
#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>
#include <stdlib.h>

static vulkan_context context;

b8 vulkan_backend_init(struct renderer_backend *backend, const char *application_name, struct window_context *platform_state){

    context.allocator = 0;

    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.apiVersion = VK_API_VERSION_1_2;
    app_info.pApplicationName = application_name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "dhemitus";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    u32 ext_count = 0;
    const char * const *extensions = SDL_Vulkan_GetInstanceExtensions(&ext_count);
    if(!extensions){
        LOG_ERROR("fetch vulkan window extensions failed");
        return false;
    }

    u32 total_ext_count = ext_count + 1;
    const char **final_extensions = malloc(sizeof(char *) * total_ext_count);
    for(u32 i = 0; i < ext_count; ++i){
        final_extensions[i] = extensions[i];
    }
    final_extensions[ext_count] = "VK_KHR_portability_enumeration";

    VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = total_ext_count;
    create_info.ppEnabledExtensionNames = final_extensions;
    create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = 0;

    VkResult result = vkCreateInstance(&create_info, context.allocator, &context.instance);
    free(final_extensions);
    if(result != VK_SUCCESS){
        LOG_ERROR("create instance failed %u", result);
        return false;
    }

    LOG_INFO("create vulkan instance succeess");
    return true;
}
void vulkan_backend_destroy(struct renderer_backend *backend){
    
}
void vulkan_renderer_resized(struct renderer_backend *backend, u16 width, u16 height){

}
b8 vulkan_begin_frame(struct renderer_backend *backend, u64 delta_time){
    return true;
}

b8 vulkan_end_frame(struct renderer_backend *backend, u64 delta_time){
    return true;
}
