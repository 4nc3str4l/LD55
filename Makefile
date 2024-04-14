# Common variables
SRC_DIR := src
DIST_DIR := dist
RAYLIB_DIR := ./dependencies/raylib/src

# Automatically list all source (.cpp) and header (.h) files
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
HEADERS := $(wildcard $(SRC_DIR)/*.h)

LIBS := -I$(RAYLIB_DIR) -L$(RAYLIB_DIR) -lraylib

# Native Options
CC := g++  # Cambiado de gcc a g++
CFLAGS := -Wall -I$(RAYLIB_DIR) -L. -lraylib -lm -lGL -lpthread -ldl -lrt -lX11
TARGET_NATIVE := $(DIST_DIR)/ludum_dare_55

# Emscripten Options
EMCC := emcc
EMFLAGS := -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 -s ALLOW_MEMORY_GROWTH=1 -s FORCE_FILESYSTEM=1 -s ASSERTIONS=1 -s STACK_SIZE=131072 --preload-file resources@/ -DPLATFORM_WEB
TARGET_WEB := $(DIST_DIR)/game.js

.PHONY: all web native clean

# Default target
all: native

# Ensure the distribution directory exists
$(shell mkdir -p $(DIST_DIR))

# Web target
web: $(TARGET_WEB)

$(TARGET_WEB): $(SOURCES) $(HEADERS)
	$(EMCC) $(SOURCES) -o $(TARGET_WEB) $(EMFLAGS) --preload-file resources $(LIBS)
	cp template.html $(DIST_DIR)/index.html

# Native target
native: $(TARGET_NATIVE)

$(TARGET_NATIVE): $(SOURCES) $(HEADERS)
	$(CC) -o $(TARGET_NATIVE) $(SOURCES) $(CFLAGS)

# Watch command
watch:
	@while inotifywait -e close_write $(SRC_DIR); do \
		make web; \
	done

# Run command for native application
run: native
	./$(TARGET_NATIVE)

# Clean command
clean:
	rm -f $(DIST_DIR)/*
