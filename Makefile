CC=gcc
CFLAGS=-Wall -Werror -Wextra -pedantic -std=c11 \
		-Wno-implicit-fallthrough -Wno-unused-variable -Wno-unused-parameter -Wno-unused-value
DEFS=-D_USE_MATH_DEFINES

SRCS=src/*.c  include/olc.c
INCL=-Iinclude -Isrc

LIBS=-lm -lSDL2main -lSDL2 include/cjson/cJSON.c
DLLS=lib/*.dll

WINDOWS=-mwindows -lmingw32 -Llib

BUILD=build
LOGS=$(BUILD)/logs
EXE=maze


all: debug run

windows: clean
	$(CC) $(WINDOWS) $(CFLAGS) $(DEFS) $(SRCS) $(INCL) $(LINK) $(LIBS) -o $(BUILD)/$(EXE)
	cp $(DLLS) $(BUILD)

linux: clean
	$(CC) $(CFLAGS) $(DEFS) $(SRCS) `sdl2-config --cflags --libs` $(INCL) $(LINK) $(LIBS) -o $(BUILD)/$(EXE)

run:
	PWD=$(PWD)/$(BUILD) $(BUILD)/$(EXE) 1>$(LOGS)/output.log 2>$(LOGS)/error.log

debug: clean
	$(CC) $(WINDOWS) $(CFLAGS) $(DEFS) -DDEBUG $(SRCS) $(INCL) $(LINK) $(LIBS) -o $(BUILD)/$(EXE)
	cp $(DLLS) $(BUILD)

bake:
	rm -rf $(BUILD)/bin/baker
	$(CC) $(WINDOWS) $(CFLAGS) $(DEFS) src/baker/baker.c $(INCL) $(LINK) $(LIBS) -lSDL2_image -lSDL2_ttf -o $(BUILD)/bin/baker
	cp $(DLLS) $(BUILD)/bin
	2>&1 1>& ./build/bin/out.log $(BUILD)/bin/baker -i $(PWD)/$(BUILD)/assets/images/* -f200 $(PWD)/$(BUILD)/assets/fonts/* -o $(PWD)/$(BUILD)/assets/baked

clean:
	rm -rf $(BUILD)/$(EXE)
