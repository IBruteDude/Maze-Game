CC=gcc
CFLAGS=-mwindows -Wall -Werror -Wextra -pedantic -std=c11 \
		-Wno-implicit-fallthrough -Wno-unused-variable -Wno-unused-parameter -Wno-unused-value
DEFS=-Dmain=SDL_main -D_USE_MATH_DEFINES

SRCS=src/*.c
INCL=-Iinclude -Isrc
LINK=-Llib

LIBS=-lmingw32 -lpthread -lm -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf include/cjson/cJSON.c include/olc.c
DLLS=lib/*.dll

BUILD=build
LOGS=$(BUILD)/logs
EXE=maze


all: debug run

build: clean
	$(CC) $(CFLAGS) $(DEFS) $(SRCS) $(INCL) $(LINK) $(LIBS) -o $(BUILD)/$(EXE)
	cp $(DLLS) $(BUILD)

run:
	PWD=$(PWD)/$(BUILD) $(BUILD)/$(EXE) 1>$(LOGS)/output.log 2>$(LOGS)/error.log

debug: clean
	$(CC) $(WINDOWS) $(CFLAGS) $(DEFS) -DDEBUG $(SRCS) $(INCL) $(LINK) $(LIBS) -o $(BUILD)/$(EXE)
	cp $(DLLS) $(BUILD)

clean:
	rm -rf $(BUILD)/$(EXE)
