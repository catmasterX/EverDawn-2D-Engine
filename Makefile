CC = gcc
CFLAGS = -O2 -Wall -Wextra -pedantic # Añade optimización y advertencias
INC_EXT_DIR = -I external/include
INC_INT_DIR = -I internal
LIB_DIR = -L external/lib
CORE_DIR = $(wildcard src/core/*.c)
SCENES_DIR = $(wildcard src/scenes/*.c)
BIN = bin/EverDawn
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lSDL2_net

all: $(BIN)

$(BIN): $(CORE_DIR) $(SCENES_DIR)
	$(CC) $(CFLAGS) $(INC_EXT_DIR) $(INC_INT_DIR) $(LIB_DIR) -o $@ $^ $(LIBS)

clean:
	rm -f $(BIN)