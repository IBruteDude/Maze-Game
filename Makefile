CC=gcc
CFLAGS=-Wall -Werror -Wextra -pedantic -mwindows -Wno-implicit-fallthrough
DEFS=-Dmain=SDL_main -D_USE_MATH_DEFINES

SRCS=src/*.c
INCL=-Iinclude
LINK=-Llib

LIBS=-lpthread -lmingw32 -lSDL2main -lSDL2 -lSDL2_image.dll -lSDL2_ttf.dll include/cjson/cJSON.c
DLLS=lib/*.dll

BUILD=build
LOGS=$(BUILD)/logs
EXE=maze


all: build run

build: clean
	$(CC) $(CFLAGS) $(DEFS) $(SRCS) $(INCL) $(LINK) $(LIBS) -o $(BUILD)/$(EXE)
	cp $(DLLS) $(BUILD)

run:
	PWD=/d/Projects/alx/Projects/Maze-project/$(BUILD) $(BUILD)/$(EXE) 1>$(LOGS)/output.log 2>$(LOGS)/error.log

debug: run
	echo -e "\n\n\n\n--------------------------------------------------------------------------------\n"\
	"output log:-\n"\
	"--------------------------------------------------------------------------------\n"\
	"`cat $(LOGS)/output.log`"\
	"\n--------------------------------------------------------------------------------\n"\
	"error log:-\n"\
	"--------------------------------------------------------------------------------\n"\
	"`cat $(LOGS)/error.log`"

clean:
	rm -rf $(BUILD)/$(EXE)


# gcc -Wall -Werror -Wextra -pedantic src/main.cpp -o build/main -I./include/ -L./lib/ -lmingw32 -lSDL2main -lSDL2 -mwindows

# /d/Programs/mingw64/bin/gcc.exe -Wall -Werror -Wextra -pedantic  src/main.c -o build/main.exe -I./include/ -L./lib/ -lmingw32 -lSDL2main -lSDL2 -mwindows
# $(CC) $(CFLAGS) $(SRC) -o $(TARGET) `sdl2-config --cflags --libs` -I/d/Programs/mingw64/include
