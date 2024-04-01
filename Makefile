CC=gcc
CFLAGS=-mwindows -Wall -Werror -Wextra -pedantic \
	-Wno-implicit-fallthrough -Wno-unused-variable -Wno-unused-parameter -Wno-unused-value
DEFS=-Dmain=SDL_main -D_USE_MATH_DEFINES

SRCS=src/*.c
INCL=-Iinclude
LINK=-Llib

LIBS=-lpthread -lmingw32 -lSDL2main -lSDL2 -lSDL2_image.dll -lSDL2_ttf.dll include/cjson/cJSON.c
DLLS=lib/*.dll

BUILD=build
LOGS=$(BUILD)/logs
EXE=maze


all: windows run

windows: clean
	$(CC) $(CFLAGS) $(DEFS) $(SRCS) $(INCL) $(LINK) $(LIBS) -o $(BUILD)/$(EXE)
	cp $(DLLS) $(BUILD)

linux: 
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) `sdl2-config --cflags --libs` -I/d/Programs/mingw64/include

run:
	PWD=$(PWD)/$(BUILD) $(BUILD)/$(EXE) 1>$(LOGS)/output.log 2>$(LOGS)/error.log

debug: run
	echo -e \
	"\n--------------------------------------------------------------------------------\n"\
	"output log:-"\
	"\n--------------------------------------------------------------------------------\n"\
	"`cat $(LOGS)/output.log`"\
	"\n--------------------------------------------------------------------------------\n"\
	"error log:-"\
	"\n--------------------------------------------------------------------------------\n"\
	"`cat $(LOGS)/error.log`"

clean:
	rm -rf $(BUILD)/$(EXE)
