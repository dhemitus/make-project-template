APP_ID        := com.engine.vulkanapp
APP_NAME      := VulkanApp.app
VERSION       := 1.0.0
EXC_NAME      := app_vulkan
# Define core absolute system tools

export CC     := /usr/bin/clang
export CXX    := /usr/bin/clang++
export AR     := ar
export ARFLAGS := rcs
GLSLC        := glslc
CLANG_FORMAT := clang-format

# This forces Clang to match your Mac's exact native processor architecture (arm64 or x86_64)
NATIVE_ARCH := -arch $(shell uname -m)

# Build Profiles (Clean and exported safely to external tools)
BUILD ?= release
ifeq ($(BUILD),debug)
    export CFLAGS   := $(NATIVE_ARCH) -Wall -Wextra -pedantic -std=c17 -g -O0 -DDEBUG -fsanitize=address
    export CXXFLAGS := $(NATIVE_ARCH) -Wall -Wextra -std=c++17 -g -O0 -DDEBUG -fsanitize=address
else
    export CFLAGS   := $(NATIVE_ARCH) -Wall -Wextra -pedantic -std=c17 -O3 -DNDEBUG
    export CXXFLAGS := $(NATIVE_ARCH) -Wall -Wextra -std=c++17 -O3 -DNDEBUG
endif

# Absolute Paths Configuration
ROOT_DIR     := $(shell pwd)
BUILD_DIR    := $(ROOT_DIR)/build
BIN_DIR      := $(BUILD_DIR)/bin
EXT_DIR      := $(ROOT_DIR)/external
SHADER_BUILD := $(BUILD_DIR)/shaders
ASSETS_DIR   := $(ROOT_DIR)/assets

# --- AUTOMATED APP SOURCE AND OBJECT TRACKING MAPS ---
APP_SRCS    := $(wildcard app/src/*.c)
APP_OBJS    := $(patsubst app/src/%.c,$(BUILD_DIR)/obj/%.o,$(APP_SRCS))

TARGET      := $(BIN_DIR)/$(EXC_NAME)
TEST_TARGET := $(BIN_DIR)/test_runner

# System Vulkan Integration Configs
VULKAN_CFLAGS := $(shell pkg-config --cflags vulkan 2>/dev/null)
VULKAN_LIBS   := $(shell pkg-config --libs vulkan 2>/dev/null || echo "-lvulkan")

# Vendor Dependency Inclusion Configurations
SDL_SRC_DIR   := $(EXT_DIR)/SDL3
SDL_BUILD_DIR := $(BUILD_DIR)/sdl_build
SDL_INC       := -I$(SDL_BUILD_DIR)/include

# TRAP 1 FIX: Changed tracking from .so to native macOS .dylib extension layout
SDL_DYLIB     := $(SDL_BUILD_DIR)/lib/libSDL3.dylib

GLFW_SRC_DIR   := $(EXT_DIR)/GLFW
GLFW_BUILD_DIR := $(BUILD_DIR)/glfw_build
GLFW_INC       := -I$(GLFW_BUILD_DIR)/include

IMGUI_SRC_DIR := $(EXT_DIR)/imgui
IMGUI_LIB     := $(BUILD_DIR)/libimgui.a
IMGUI_INC     := -I$(IMGUI_SRC_DIR) -I$(IMGUI_SRC_DIR)/backends

# TRAP 3 FIX: Appended mandatory macOS native operating system system frameworks configuration tags
APPLE_FRAMEWORKS := -framework Cocoa -framework IOKit -framework CoreVideo -framework CoreAudio \
                    -framework AudioToolbox -framework QuartzCore -framework Metal -framework Foundation

EXT_CFLAGS    := $(VULKAN_CFLAGS) $(SDL_INC) $(GLFW_INC) $(IMGUI_INC)
EXT_LIBS      := -L$(SDL_BUILD_DIR)/lib -lSDL3 -L$(GLFW_BUILD_DIR)/lib -lglfw3 $(IMGUI_LIB) $(VULKAN_LIBS) $(APPLE_FRAMEWORKS) -lm

# Consolidated Engine Dynamic Library Properties
LIB_ENGINE    := $(ROOT_DIR)/libs/libengine.dylib
INTERNAL_LIBS := -L$(ROOT_DIR)/libs -lengine

export GLOBAL_INC := -I$(ROOT_DIR)/include $(EXT_CFLAGS)
LOCAL_INC  := -I$(ROOT_DIR)/libs/common/include -I$(ROOT_DIR)/libs/runtime/include -I$(ROOT_DIR)/libs/core/include

# Shader Automation Source Maps
VERT_SHADERS := $(wildcard shaders/*.vert)
FRAG_SHADERS := $(wildcard shaders/*.frag)
SPV_OUTPUTS  := $(VERT_SHADERS:shaders/%.vert=$(SHADER_BUILD)/%.vert.spv) \
                $(FRAG_SHADERS:shaders/%.frag=$(SHADER_BUILD)/%.frag.spv)

.PHONY: all clean run subdirs deps init_submodules compile_commands

all: $(TARGET) shaders icon

init_submodules:
	@if [ ! -f "$(SDL_SRC_DIR)/CMakeLists.txt" ] || [ ! -f "$(GLFW_SRC_DIR)/CMakeLists.txt" ]; then \
		git submodule update --init --recursive; \
	fi

deps: init_submodules $(IMGUI_LIB)

# TRAP 1 FIX: Updated dependency target line to search for native Apple library format
$(SDL_DYLIB):
	@mkdir -p $(SDL_BUILD_DIR)
	cd $(SDL_BUILD_DIR) && cmake $(SDL_SRC_DIR) -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=. -DSDL_TESTS=OFF -DSDL_EXAMPLES=OFF -DSDL_STATIC=OFF -DSDL_SHARED=ON
	$(MAKE) -C $(SDL_BUILD_DIR) install

$(GLFW_BUILD_DIR)/lib/libglfw3.a:
	@mkdir -p $(GLFW_BUILD_DIR)
	cd $(GLFW_BUILD_DIR) && cmake $(GLFW_SRC_DIR) -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=. -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DGLFW_INSTALL=ON
	$(MAKE) -C $(GLFW_BUILD_DIR) install

# TRAP 2 REMINDER: Verify these precise .cpp file strings exist inside your external/imgui/backends/ directory!
IMGUI_OBJS := $(BUILD_DIR)/imgui_obj/imgui.o $(BUILD_DIR)/imgui_obj/imgui_draw.o $(BUILD_DIR)/imgui_obj/imgui_tables.o $(BUILD_DIR)/imgui_obj/imgui_widgets.o $(BUILD_DIR)/imgui_obj/imgui_demo.o $(BUILD_DIR)/imgui_obj/imgui_impl_sdl3.o $(BUILD_DIR)/imgui_obj/imgui_impl_glfw.o $(BUILD_DIR)/imgui_obj/imgui_impl_vulkan.o
$(BUILD_DIR)/imgui_obj/%.o: $(IMGUI_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(IMGUI_INC) $(SDL_INC) $(GLFW_INC) $(VULKAN_CFLAGS) -c $< -o $@
$(BUILD_DIR)/imgui_obj/%.o: $(IMGUI_SRC_DIR)/backends/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(IMGUI_INC) $(SDL_INC) $(GLFW_INC) $(VULKAN_CFLAGS) -c $< -o $@
$(IMGUI_LIB): $(SDL_DYLIB) $(GLFW_BUILD_DIR)/lib/libglfw3.a $(IMGUI_OBJS)
	$(AR) $(ARFLAGS) $@ $(IMGUI_OBJS)

subdirs: deps
	@mkdir -p $(BUILD_DIR)/obj
	$(MAKE) -C libs

# --- THE NATIVE AWK COMPILATION DATABASE MERGER ---
compile_commands:
	@echo "Updating Neovim LSP tracking database..."
	@echo "[" > compile_commands.json
	@cat $(BUILD_DIR)/obj/*.json.tmp 2>/dev/null | awk '{printf "%s%s", (NR==1?"":",\n"), $$0}' >> compile_commands.json
	@echo "\n]" >> compile_commands.json

# --- THE AD-HOC SIGNED DEVELOPMENT RUNTIME FIX ---
$(TARGET): $(APP_OBJS) subdirs deps
	@mkdir -p $(BIN_DIR)
	@echo "Linking master application target with portable path parameters..."
	
	# 1. Clean and register internal tracking IDs mapping properties
	@install_name_tool -id $(ROOT_DIR)/libs/libengine.dylib $(ROOT_DIR)/libs/libengine.dylib 2>/dev/null || true
	@install_name_tool -id $(SDL_BUILD_DIR)/lib/libSDL3.dylib $(SDL_BUILD_DIR)/lib/libSDL3.dylib 2>/dev/null || true
	
	# 2. Copy the system Vulkan loader into our local libs/ directory
	@VULKAN_SYSTEM_DYLIB=$$(pkg-config --variable=libdir vulkan 2>/dev/null)/libvulkan.1.dylib; \
	if [ -f "$$VULKAN_SYSTEM_DYLIB" ]; then \
		cp "$$VULKAN_SYSTEM_DYLIB" "$(ROOT_DIR)/libs/"; \
	elif [ -f "/opt/homebrew/lib/libvulkan.1.dylib" ]; then \
		cp "/opt/homebrew/lib/libvulkan.1.dylib" "$(ROOT_DIR)/libs/"; \
	else \
		cp "/usr/local/lib/libvulkan.1.dylib" "$(ROOT_DIR)/libs/"; \
	fi
	@install_name_tool -id @rpath/libvulkan.1.dylib "$(ROOT_DIR)/libs/libvulkan.1.dylib" 2>/dev/null || true
	
	# --- THE KERNEL SECURITY SIGNATURE OVERRIDE FIX ---
	# We force an ad-hoc code signature onto the newly copied library. 
	# This stops the macOS kernel from instantly firing a SIGKILL: 9 on execution passes!
	@echo "Signing local Vulkan development binary constraints..."
	@codesign --force --sign - "$(ROOT_DIR)/libs/libvulkan.1.dylib" 2>/dev/null || true
	
	# 3. Link the master executable passing the active objects array
	$(CXX) $(CXXFLAGS) $(APP_OBJS) $(INTERNAL_LIBS) $(EXT_LIBS) \
		-Wl,-rpath,$(GLFW_BUILD_DIR)/lib \
		-Wl,-rpath,$(SDL_BUILD_DIR)/lib \
		-Wl,-rpath,$(ROOT_DIR)/libs -o $@
		
	# 4. Force the application binary to track the local Vulkan loader instead of global links
	@install_name_tool -change @rpath/libvulkan.1.dylib @rpath/libvulkan.1.dylib $(TARGET) 2>/dev/null || true
	@install_name_tool -change libvulkan.1.dylib @rpath/libvulkan.1.dylib $(TARGET) 2>/dev/null || true
	@install_name_tool -change /opt/homebrew/lib/libvulkan.1.dylib @rpath/libvulkan.1.dylib $(TARGET) 2>/dev/null || true
	
	@install_name_tool -change libSDL3.0.dylib $(SDL_BUILD_DIR)/lib/libSDL3.dylib $(TARGET) 2>/dev/null || true
	@install_name_tool -change @rpath/libSDL3.0.dylib $(SDL_BUILD_DIR)/lib/libSDL3.dylib $(TARGET) 2>/dev/null || true
	@install_name_tool -change libengine.dylib $(ROOT_DIR)/libs/libengine.dylib $(TARGET) 2>/dev/null || true
	@install_name_tool -change @rpath/libengine.dylib $(ROOT_DIR)/libs/libengine.dylib $(TARGET) 2>/dev/null || true
	@$(MAKE) compile_commands

DEPFLAGS = -MT $@ -MMD -MP -MF $(BUILD_DIR)/obj/$*.d

# --- THE AUTOMATED APP PATTERN COMPILATION FIX ---
# Wipes out hardcoded file restrictions. Safely captures new files dynamically!
$(BUILD_DIR)/obj/%.o: app/src/%.c subdirs shaders
	@mkdir -p $(BUILD_DIR)/obj
	$(CC) $(DEPFLAGS) -MJ $(BUILD_DIR)/obj/$*.json.tmp $(CFLAGS) $(LOCAL_INC) $(GLOBAL_INC) -c $< -o $@

run: $(TARGET) shaders
	@echo "Signing master desktop binary package layout permissions..."
	@codesign --force --deep --sign - $(TARGET)
	@sleep 0.5
	@echo "Launching Signed Vulkan Engine Workspace..."
	@$(abspath $(TARGET))

clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) -C libs clean

# Modular Make Targets Modules
include make/icon.mk
include make/shader.mk
include make/test.mk
include make/package.mk
include make/android.mk
include make/ios.mk

-include $(wildcard $(BUILD_DIR)/obj/*.d)
