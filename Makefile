# Define core tools
export CC     := /usr/bin/clang
export CXX    := /usr/bin/clang++
export AR     := ar
export ARFLAGS := rcs
GLSLC        := glslc
CLANG_FORMAT := clang-format

# Build Profiles (Usage: "make" or "make BUILD=debug")
BUILD ?= release
ifeq ($(BUILD),debug)
    # Added -fsanitize=address to both trackers
    export CFLAGS   := -Wall -Wextra -pedantic -std=c17 -g -O0 -DDEBUG -fsanitize=address
    export CXXFLAGS := -Wall -Wextra -std=c++17 -g -O0 -DDEBUG -fsanitize=address
else
    export CFLAGS   := -Wall -Wextra -pedantic -std=c17 -O3 -DNDEBUG
    export CXXFLAGS := -Wall -Wextra -std=c++17 -O3 -DNDEBUG
endif

# Absolute paths
ROOT_DIR     := $(shell pwd)
BUILD_DIR    := $(ROOT_DIR)/build
BIN_DIR      := $(BUILD_DIR)/bin
EXT_DIR      := $(ROOT_DIR)/external
ASSETS_DIR   := $(ROOT_DIR)/assets
SHADER_BUILD := $(BUILD_DIR)/shaders

# Final Output Binaries
TARGET      := $(BIN_DIR)/app_main
TEST_TARGET := $(BIN_DIR)/test_runner

# System Vulkan Flags
VULKAN_CFLAGS := $(shell pkg-config --cflags vulkan 2>/dev/null)
VULKAN_LIBS   := $(shell pkg-config --libs vulkan 2>/dev/null || echo "-lvulkan")

# Third-Party Dependencies Paths
SDL_SRC_DIR   := $(EXT_DIR)/SDL3
SDL_BUILD_DIR := $(BUILD_DIR)/sdl_build
SDL_INC       := -I$(SDL_BUILD_DIR)/include

GLFW_SRC_DIR   := $(EXT_DIR)/GLFW
GLFW_BUILD_DIR := $(BUILD_DIR)/glfw_build
GLFW_INC       := -I$(GLFW_BUILD_DIR)/include

IMGUI_SRC_DIR := $(EXT_DIR)/imgui
IMGUI_LIB     := $(BUILD_DIR)/libimgui.a
IMGUI_INC     := -I$(IMGUI_SRC_DIR) -I$(IMGUI_SRC_DIR)/backends

# Consolidated Third-Party Flags
EXT_CFLAGS    := $(VULKAN_CFLAGS) $(SDL_INC) $(GLFW_INC) $(IMGUI_INC)
EXT_LIBS      := -L$(SDL_BUILD_DIR)/lib -lSDL3 -L$(GLFW_BUILD_DIR)/lib -lglfw3 $(IMGUI_LIB) $(VULKAN_LIBS) -lm

# Consolidated Internal Engine Library Config
LIB_ENGINE    := $(ROOT_DIR)/libs/libengine.dylib
INTERNAL_LIBS := -L$(ROOT_DIR)/libs -lengine

export GLOBAL_INC := -I$(ROOT_DIR)/include $(EXT_CFLAGS)
LOCAL_INC  := -I$(ROOT_DIR)/libs/common/include -I$(ROOT_DIR)/libs/core/include

# Shader Automation Source Maps (Searches inside shaders/)
VERT_SHADERS := $(wildcard shaders/*.vert)
FRAG_SHADERS := $(wildcard shaders/*.frag)
SPV_OUTPUTS  := $(VERT_SHADERS:shaders/%.vert=$(SHADER_BUILD)/%.vert.spv) \
                $(FRAG_SHADERS:shaders/%.frag=$(SHADER_BUILD)/%.frag.spv)

.PHONY: all clean run subdirs deps init_submodules 

all: $(TARGET) shaders icon

init_submodules:
	@if [ ! -f "$(SDL_SRC_DIR)/CMakeLists.txt" ] || [ ! -f "$(GLFW_SRC_DIR)/CMakeLists.txt" ]; then \
		git submodule update --init --recursive; \
	fi

deps: init_submodules $(IMGUI_LIB)

$(SDL_BUILD_DIR)/lib/libSDL3.so:
	@mkdir -p $(SDL_BUILD_DIR)
	cd $(SDL_BUILD_DIR) && cmake $(SDL_SRC_DIR) -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=. -DSDL_TESTS=OFF -DSDL_EXAMPLES=OFF -DSDL_STATIC=OFF -DSDL_SHARED=ON
	$(MAKE) -C $(SDL_BUILD_DIR) install

$(GLFW_BUILD_DIR)/lib/libglfw3.a:
	@mkdir -p $(GLFW_BUILD_DIR)
	cd $(GLFW_BUILD_DIR) && cmake $(GLFW_SRC_DIR) -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=. -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DGLFW_INSTALL=ON
	$(MAKE) -C $(GLFW_BUILD_DIR) install

IMGUI_OBJS := $(BUILD_DIR)/imgui_obj/imgui.o $(BUILD_DIR)/imgui_obj/imgui_draw.o $(BUILD_DIR)/imgui_obj/imgui_tables.o $(BUILD_DIR)/imgui_obj/imgui_widgets.o $(BUILD_DIR)/imgui_obj/imgui_demo.o $(BUILD_DIR)/imgui_obj/imgui_impl_sdl3.o $(BUILD_DIR)/imgui_obj/imgui_impl_glfw.o $(BUILD_DIR)/imgui_obj/imgui_impl_vulkan.o
$(BUILD_DIR)/imgui_obj/%.o: $(IMGUI_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(IMGUI_INC) $(SDL_INC) $(GLFW_INC) $(VULKAN_CFLAGS) -c $< -o $@
$(BUILD_DIR)/imgui_obj/%.o: $(IMGUI_SRC_DIR)/backends/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(IMGUI_INC) $(SDL_INC) $(GLFW_INC) $(VULKAN_CFLAGS) -c $< -o $@
$(IMGUI_LIB): $(SDL_BUILD_DIR)/lib/libSDL3.so $(GLFW_BUILD_DIR)/lib/libglfw3.a $(IMGUI_OBJS)
	$(AR) $(ARFLAGS) $@ $(IMGUI_OBJS)

# Call internal subfolder library build script
subdirs: deps
	$(MAKE) -C libs

# Compile Main App Execution Scope
$(BUILD_DIR)/main.o: app/src/main.c subdirs
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LOCAL_INC) $(GLOBAL_INC) -c $< -o $@

# Link final binary bundle 
$(TARGET): $(BUILD_DIR)/main.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< $(INTERNAL_LIBS) $(EXT_LIBS) \
		-Wl,-rpath,$(SDL_BUILD_DIR)/lib \
		-Wl,-rpath,$(ROOT_DIR)/libs -o $@

run: $(TARGET) shaders
	@./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) -C libs clean

include make/icon.mk
include make/shader.mk
include make/test.mk
include make/package.mk
include make/android.mk
