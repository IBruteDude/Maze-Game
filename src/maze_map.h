#ifndef MAZE_MAP_H
#define MAZE_MAP_H

#include "common.h"

typedef struct map_s
{
	int w, h;
	byte_vec data;
} map_t;

typedef enum map_blk_e
{
	FLOOR = '0',
	WALL = '1',
} map_blk_t;

bool map_load(map_t *map, const char *mapfile);

void map_free(map_t *map);

map_blk_t map_get(map_t *map, int x, int y);

#endif
