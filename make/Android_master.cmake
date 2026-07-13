cmake_minimum_required(VERSION 3.22)
project(vulkan_forge_android_jni C CXX)

# Ensure SDL3 shared library targets are imported natively
if(NOT TARGET SDL3-shared)
    add_subdirectory(SDL)
endif()

# --- 1. COMPILE YOUR MODULAR ENGINE CORE ---
add_library(engine SHARED)

file(GLOB COMMON_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../libs/common/src/*.c")
file(GLOB CORE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../libs/core/src/*.c")
target_sources(engine PRIVATE ${COMMON_SRCS} ${CORE_SRCS})

target_include_directories(engine PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../libs/common/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../libs/core/include"
)
target_link_libraries(engine PRIVATE log android m)

# --- 2. COMPILE APPS MAIN ENTRY POINT ---
# Changed target name to vulkan_forge_app to completely break duplicate token bugs!
add_library(vulkan_forge_app SHARED)

# Track down the mandatory JNI bootstrap file inside your SDL3 submodule
file(GLOB_RECURSE SDL_JNI_BRIDGE "${CMAKE_CURRENT_SOURCE_DIR}/../../../../external/SDL3/src/main/android/*.c")

# Compile YOUR actual desktop app/src/main.c file AND the JNI bootstrap bridge together
target_sources(vulkan_forge_app PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../app/src/main.c"
    ${SDL_JNI_BRIDGE}
)

target_include_directories(vulkan_forge_app PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../libs/common/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../libs/core/include"
)

# Link your custom core engine and SDL3 frameworks straight to your unique target module
target_link_libraries(vulkan_forge_app PRIVATE SDL3-shared engine log android m)

