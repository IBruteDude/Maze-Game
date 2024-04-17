CC=gcc
CFLAGS=-mwindows -Wall -Werror -Wextra -pedantic -std=c11 \
		-Wno-implicit-fallthrough -Wno-unused-variable -Wno-unused-parameter -Wno-unused-value
DEFS=-Dmain=SDL_main -D_USE_MATH_DEFINES

SRCS=src/*.c  include/olc.c
INCL=-Iinclude -Isrc
LINK=-Llib

LIBS=-lmingw32 -lm -lSDL2main -lSDL2 include/cjson/cJSON.c
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

bake:
	rm -rf $(BUILD)/bin/baker.exe
	$(CC) $(WINDOWS) $(CFLAGS) $(DEFS) src/baker/baker.c $(INCL) $(LINK) $(LIBS) -lSDL2_image -lSDL2_ttf -o $(BUILD)/bin/baker.exe
	cp $(DLLS) $(BUILD)/bin
	2>&1 1>& ./build/bin/out.log $(BUILD)/bin/baker.exe -i $(PWD)/$(BUILD)/assets/images/* -f200 $(PWD)/$(BUILD)/assets/fonts/* -o $(PWD)/$(BUILD)/assets/baked

clean:
	rm -rf $(BUILD)/$(EXE)
