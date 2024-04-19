#include "maze_map.h"

bool map_load(map_t *map, const char *mapfile)
{
	FILE *f;
	char pathbuf[PATH_MAX] = "", buf[1024];
	int len = 0, n = 0, i = 0;

	sprintf(pathbuf, "%s/%s", getenv("PWD"), mapfile), f = fopen(pathbuf, "r");
	if (!f)
		return (false);
	da_init(map->data);
	while ((n = fread(buf, 1, 1024, f)))
		da_addn(map->data, buf, n);
	len = da_count(map->data);
	while (i < len && da_get(map->data, i) != '\n')
		i++;
	map->w = ++i;
	if (len % map->w != 0)
		goto cleanup;
	while (i < len)
	{
		while (i < len && da_get(map->data, i) != '\n')
			i++;
		if (++i % map->w != 0)
		{
			fprintf(stderr, "error loading map from file: %s\n"
							"inconsistent map dimensions\n", mapfile);
			goto cleanup;
		}
	}
	map->h = len / map->w--;

	map->start_x = map->start_y = -1;
	for (int i = 0; i < map->w; i++)
		for (int j = 0; j < map->h; j++)
			if (map_get(map, i, j) == MAP_ENTERANCE && (
					((i < map->w - 1) && map_get(map, i + 1, j)) ||
					((j < map->h - 1) && map_get(map, i, j + 1))
				))
				map->start_x = i, map->start_y = j;
	
	if (map->start_x == -1)
	{
		fprintf(stderr, "error loading map from file: %s\n"
						"map has no valid enterance: \"%c\"\n", mapfile, MAP_ENTERANCE);
		goto cleanup;
	}

	fclose(f);
	return (true);
cleanup:
	if (f)
		fclose(f);
	if (map)
		da_free(map->data), free(map);
	return (false);
}

void map_free(map_t *map)
{
	da_free(map->data);
	free(map);
}

map_blk_t map_get(map_t *map, int x, int y)
{
	assert(0 <= x && x < map->w && 0 <= y && y < map->h);
	return (map->data.p[x * (map->w + 1) + y]);
}
