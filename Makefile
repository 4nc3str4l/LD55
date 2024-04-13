# Common variables
SRC_DIR=src
DIST_DIR=dist
RAYLIB_DIR=./dependencies/raylib/src

# Sources
SOURCES=$(SRC_DIR)/main.cpp $(SRC_DIR)/world.cpp

# Header files
HEADERS=$(SRC_DIR)/world.h $(SRC_DIR)/utils.h

LIBS=-I$(RAYLIB_DIR) -L$(RAYLIB_DIR) -lraylib

# Native Options
CC=g++  # Cambiado de gcc a g++
CFLAGS=-Wall -I$(RAYLIB_DIR) -L. -lraylib -lm -lGL -lpthread -ldl -lrt -lX11
TARGET_NATIVE=$(DIST_DIR)/ludum_dare_55

# Emscripten Options
EMCC=emcc
EMFLAGS=-s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 -s ALLOW_MEMORY_GROWTH=1 -s FORCE_FILESYSTEM=1 -s ASSERTIONS=1 -sSTACK_SIZE=131072 --preload-file resources@/
TARGET_WEB=$(DIST_DIR)/game.js


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

# native target
native: $(TARGET_NATIVE)

$(TARGET_NATIVE): $(SOURCES) $(HEADERS)
	$(CC) -o $(TARGET_NATIVE) $(SOURCES) $(CFLAGS)

# Watch command
watch:
	@while inotifywait -e close_write $(SRC_DIR); do \
		make web; \
	done

# clean command
clean:
	rm -f $(DIST_DIR)/*
