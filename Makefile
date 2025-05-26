# Compiler settings
CC_LINUX = gcc
CC_WIN = x86_64-w64-mingw32-gcc

CFLAGS_LINUX = -g
CFLAGS_WIN = -D__USE_MINGW_ANSI_STDIO=1 -D_WIN32 -g -Wall

SDL_FLAGS_LINUX = $(shell sdl2-config --cflags --libs)
SDL_FLAGS_WIN = $(shell /usr/x86_64-w64-mingw32/bin/sdl2-config --cflags --libs)

LDFLAGS_LINUX = -rdynamic $(SDL_FLAGS_LINUX) -lm -lGLU -lGLEW -lOpenGL
LDFLAGS_WIN = -lmingw32 $(SDL_FLAGS_WIN) -lmsvcrt -lglew32 -lopengl32 -lglu32

# Source files
SRC = zenithra_main zenithra_core zenithra_debug zenithra_movement zenithra_events zenithra_graphics zenithra_editor

SRC_DIR = dev/src
OBJ_LINUX = $(addprefix $(SRC_DIR)/, $(addsuffix .o, $(SRC)))
OBJ_WIN = $(addprefix $(SRC_DIR)/, $(addsuffix _windows.o, $(SRC)))

# Default target (Linux)
all: main

main: $(OBJ_LINUX)
	$(CC_LINUX) $(OBJ_LINUX) $(CFLAGS_LINUX) -o main $(LDFLAGS_LINUX)

# Windows target
windows: $(OBJ_WIN)
	$(CC_WIN) $(CFLAGS_WIN) $(OBJ_WIN) -o main.exe $(LDFLAGS_WIN)

# Pattern rules
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/zenithra_core.h $(SRC_DIR)/zenithra_debug.h $(SRC_DIR)/vertex_shader.h $(SRC_DIR)/fragment_shader.h
	$(CC_LINUX) -c $< -o $@

$(SRC_DIR)/%_windows.o: $(SRC_DIR)/%.c $(SRC_DIR)/zenithra_core.h $(SRC_DIR)/zenithra_debug.h $(SRC_DIR)/vertex_shader.h $(SRC_DIR)/fragment_shader.h
	$(CC_WIN) $(CFLAGS_WIN) -c $< -o $@

# Clean
clean:
	rm -f $(OBJ_LINUX) $(OBJ_WIN) main main.exe