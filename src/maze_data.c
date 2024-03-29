#include "maze_data.h"

#include "cjson/cJSON.h"

#define json_obj(obj, field) cJSON_GetObjectItem(obj, field)
#define json_num(obj, field) cJSON_GetNumberValue(json_obj(obj, field))
#define json_str(obj, field) cJSON_GetStringValue(json_obj(obj, field))

char *loadfile(const char *filename) {
	FILE *f = fopen(filename, "r");
	char *buf;
	long fsize;

	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	buf = malloc(fsize + 1);
	fread(buf, 1, fsize, f);
	buf[fsize] = '\0';

	fclose(f);
	return buf;
}

bool maze_data_load(const char *savefile)
{
	maze_game_context_t *ctx = game_ctx();
	cJSON *game_data, *player_pos;
	char *buf, pathbuf[PATH_MAX] = "";

	sprintf(pathbuf, "%s/%s", getenv("PWD"), savefile);
	buf = loadfile(pathbuf);
	game_data = cJSON_Parse(buf);
	free(buf);
	if (!game_data)
		return (false);

	player_pos = json_obj(game_data, "player");
	ctx->fps = json_num(game_data, "fps");
	ctx->pl->x = json_num(player_pos, "x"),
	ctx->pl->y = json_num(player_pos, "y"),
	ctx->pl->view = json_num(player_pos, "view");

	map_load(ctx->map, json_str(game_data, "map"));


	ctx->fz = json_num(game_data, "font_size");

	DEBUGI(ctx->fz);
	sprintf(pathbuf, "%s/%s", getenv("PWD"), json_str(game_data, "font"));
	ctx->font = TTF_OpenFont(pathbuf, ctx->fz);
	puts(pathbuf);

	if (ctx->font == NULL)
		maze_loge("TTF_OpenFont"), exit(EXIT_FAILURE);

	cJSON_Delete(game_data);
	return (!!ctx->map);
}

void maze_data_free()
{
	maze_game_context_t *ctx = game_ctx();

	map_free(ctx->map);
}
