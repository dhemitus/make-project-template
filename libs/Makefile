CC      := clang
CFLAGS  := -Wall -Wextra -pedantic -std=c17 -O2 -g -fPIC -fvisibility=hidden

# --- THE FIX: Automated Dependency Generation ---
DEPFLAGS = -MT $@ -MMD -MP -MF obj/$*.d

SRCS    := $(wildcard common/src/*.c) $(wildcard core/src/*.c)
OBJS    := $(patsubst %.c,obj/%.o,$(notdir $(SRCS)))
DEPS    := $(OBJS:.o=.d) # List of generated tracking files
TARGET  := libengine.dylib

INC_FLAGS := -Icommon/include -Icore/include $(GLOBAL_INC)

.PHONY: all clean

all: $(TARGET)

# Link the dylib
$(TARGET): $(OBJS)
	$(CC) -shared -Wl,-install_name,@rpath/$(TARGET) -o $@ $^



# Rule to compile any .c file from common/src/ into obj/
obj/%.o: common/src/%.c
	@mkdir -p obj
	$(CC) $(DEPFLAGS) $(CFLAGS) $(INC_FLAGS) -c $< -o $@

# Rule to compile any .c file from core/src/ into obj/
obj/%.o: core/src/%.c
	@mkdir -p obj
	$(CC) $(DEPFLAGS) $(CFLAGS) $(INC_FLAGS) -c $< -o $@

clean:
	rm -rf obj $(TARGET)
# --- THE FIX: Pull in header tracking configurations safely ---
-include $(DEPS)
