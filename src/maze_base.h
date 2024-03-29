#ifndef MAZE_BASE_H
#define MAZE_BASE_H

#include "common.h"
#include "maze_map.h"

#include <pthread.h>

#include "DG_dynarr.h"

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

typedef struct maze_game_context_s
{
	SDL_Renderer *rend;
	SDL_Window *win;
	TTF_Font *font;
	int fz;

	player_t *pl;
	map_t *map;
	maze_resource_stack_t *resource_stack;
	double dtmin;
	double dt;

	unsigned char raycaster;
	int hoff, voff;
} maze_game_context_t;

maze_game_context_t *game_ctx(void);

void CTOR maze_init(void);

void DTOR maze_cleanup(void);

bool maze_resource_stack_push(void *resource, void (*deallocator)(void *));

void maze_resource_stack_pop(void);

#endif
