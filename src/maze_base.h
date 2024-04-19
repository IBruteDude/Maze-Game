#ifndef MAZE_BASE_H
#define MAZE_BASE_H

#include "common.h"
#include "maze_map.h"

#define WIN_W 1600
#define WIN_H 900

#ifndef NO_CDTORS
	#define CTOR __attribute__((constructor))
	#define DTOR __attribute__((destructor))
#else
	#define CTOR
	#define DTOR
#endif

typedef struct maze_resource_s
{
	void *resource;
	void (*deallocator)(void *);
} maze_resource_t;

DG_DYNARR_TYPEDEF(maze_resource_t, maze_resource_stack_t)
DG_DYNARR_TYPEDEF(SDL_Texture *, texture_pack_t)

typedef enum maze_game_state_e
{
	MAZE_MINIMAP,
	MAZE_3D,
	MAZE_MAINSCREEN,
	MAZE_WINSCREEN,
} maze_game_state_t;

typedef struct maze_game_context_s
{
	SDL_Renderer *rend;
	SDL_Window *win;
	texture_pack_t *texs;
	SDL_Texture *char_texs[CHAR_MAX];
	char font[100];
	int fz;

	maze_game_state_t state;
	struct timespec rtstart;
	struct timespec rtend;
	player_t *pl;
	map_t *map;
	maze_resource_stack_t *resource_stack;
	bool **pl_viewed;

	struct timespec *tlast;
	double dtmin;
	double dt;
	bool capfps : 1;
	bool textured : 1;
	bool focused : 1;
	bool helpmsg : 1;
	bool fpsdisplay : 1;

	int hoff, voff;
} maze_game_context_t;

maze_game_context_t *game_ctx(void);

void CTOR maze_init(void);

void DTOR maze_cleanup(void);

bool maze_resource_stack_push(void *resource, void (*deallocator)(void *));

void maze_resource_stack_pop(void);

#endif
