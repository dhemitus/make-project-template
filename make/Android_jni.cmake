cmake_minimum_required(VERSION 3.22)

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

# --- 2. THE CHRONOLOGICAL SOURCE OVERRIDE & JNI BRIDGE VALVE ---
# Wipe out whatever relative source file the template's main target was tracking
set_property(TARGET main PROPERTY SOURCES "")

# DYNAMIC JNI LOOKUP: Search your local submodule to find the native Android startup bridge file!
file(GLOB_RECURSE SDL_JNI_BRIDGE "${CMAKE_CURRENT_SOURCE_DIR}/../../../../external/SDL3/src/main/android/*.c")

# Force CMake to compile YOUR app/src/main.c AND the mandatory JNI bootstrap connector together!
target_sources(main PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../app/src/main.c"
    ${SDL_JNI_BRIDGE}
)

# Bind your modular include tracking fields so main can see your engine headers cleanly
target_include_directories(main PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../libs/common/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../libs/core/include"
)

# Link your engine module directly to the primary main wrapper package
target_link_libraries(main PUBLIC engine)

