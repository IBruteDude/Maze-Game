#ifndef MAZE_MAP_H
#define MAZE_MAP_H

#include "common.h"

typedef struct map_s
{
	int w, h;
	int start_x, start_y;
	byte_vec data;
} map_t;

typedef enum map_blk_e
{
	MAP_FLOOR = '0',
	MAP_WALL = '1',
	MAP_ENTERANCE = '2',
	MAP_EXIT = '3',
} map_blk_t;

bool map_load(map_t *map, const char *mapfile);

void map_free(map_t *map);

map_blk_t map_get(map_t *map, int x, int y);

#endif
