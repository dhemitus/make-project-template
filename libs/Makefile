
# --- FUTURE-PROOF ENGINE CORE MAKEFILE ---
# Fully handles both C (.c) and C++ (.cpp) source paths natively for Dear ImGui!
CC       := clang
CXX      := clang++
CFLAGS   := -arch $(shell uname -m) -Wall -Wextra -pedantic -std=c17 -O2 -g -fPIC -fvisibility=hidden
CXXFLAGS := -arch $(shell uname -m) -Wall -Wextra -pedantic -std=c++17 -O2 -g -fPIC -fvisibility=hidden

DEPFLAGS = -MT $@ -MMD -MP -MF $(patsubst %.o,%.d,$@)
LSP_FLAGS = -MJ $(patsubst %.o,%.json.tmp,$@)

# Dynamically map both C and C++ files while preserving subdirectory structures
SRCS_C   := $(wildcard common/src/*.c) $(wildcard core/src/*.c)
SRCS_CXX := $(wildcard common/src/*.cpp) $(wildcard core/src/*.cpp)

OBJS     := $(patsubst %.c,obj/%.o,$(SRCS_C)) $(patsubst %.cpp,obj/%.o,$(SRCS_CXX))
DEPS     := $(OBJS:.o=.d)

ROOT_DIR       := $(shell cd .. && pwd)
SDL_BUILD_DIR  := $(ROOT_DIR)/build/sdl_build
TARGET         := libengine.dylib
INC_FLAGS      := -Icommon/include -Icore/include -I../include $(GLOBAL_INC)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -shared -Wl,-install_name,@rpath/$(TARGET) \
		-L$(SDL_BUILD_DIR)/lib -lSDL3 \
		-o $@ $^
	@mkdir -p ../build/obj
	# Copy all deep nested json chunks up to the root build tree safely
	@find obj -name "*.json.tmp" -exec cp {} ../build/obj/ \; 2>/dev/null || true

# Rule pattern to compile common and core C source files (.c)
obj/common/src/%.o: common/src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(DEPFLAGS) $(LSP_FLAGS) $(CFLAGS) $(INC_FLAGS) -c $< -o $@

obj/core/src/%.o: core/src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(DEPFLAGS) $(LSP_FLAGS) $(CFLAGS) $(INC_FLAGS) -c $< -o $@

# Rule pattern to compile common and core C++ source files (.cpp) for ImGui
obj/common/src/%.o: common/src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(DEPFLAGS) $(LSP_FLAGS) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@

obj/core/src/%.o: core/src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(DEPFLAGS) $(LSP_FLAGS) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@

clean:
	rm -rf obj $(TARGET)

-include $(DEPS)
