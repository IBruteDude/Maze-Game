#include "maze_data.h"

#include "cjson/cJSON.h"

#define OBJ(obj, field) cJSON_GetObjectItem(obj, field)

#define BOOL(obj) cJSON_IsTrue(obj)
#define NUM(obj) cJSON_GetNumberValue(obj)
#define STR(obj) cJSON_GetStringValue(obj)
#define ARR_GET(obj, idx)	cJSON_GetArrayItem(obj, idx)
#define ARR_SIZE(obj)	cJSON_GetArraySize(obj)

#define json_bool(obj, field) BOOL(OBJ(obj, field))
#define json_num(obj, field) NUM(OBJ(obj, field))
#define json_str(obj, field) STR(OBJ(obj, field))
#define json_arr_get(obj, field, idx)	ARR_GET(OBJ(obj, field), idx)
#define json_arr_size(obj, field)	ARR_SIZE(OBJ(obj, field))

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
	cJSON *game_data, *player;
	char *buf, pathbuf[PATH_MAX] = "";

	sprintf(pathbuf, "%s/%s", getenv("PWD"), savefile);
	buf = loadfile(pathbuf), game_data = cJSON_Parse(buf), free(buf);
	if (!game_data)
		return (false);

	player = OBJ(game_data, "player");
	ctx->dtmin = 1 / json_num(game_data, "fpsmax"), ctx->dt = ctx->dtmin;
	ctx->capfps = json_bool(game_data, "capfps");
	ctx->pl->x = json_num(player, "x"), ctx->pl->y = json_num(player, "y"),
	ctx->pl->view = json_num(player, "view");
	ctx->pl->speed = json_num(player, "speed");
	ctx->pl->xvel = ctx->pl->yvel = 0;

	map_load(ctx->map, json_str(game_data, "map"));

	ctx->fz = json_num(game_data, "font_size");

	sprintf(pathbuf, "%s/%s", getenv("PWD"), json_str(game_data, "font"));
	ctx->font = TTF_OpenFont(pathbuf, ctx->fz);

	if (ctx->font == NULL)
		maze_loge("TTF_OpenFont"), exit(EXIT_FAILURE);

	int tex_num = json_arr_size(game_data, "textures");

	for (int i = 0; i < tex_num; i++)
	{
		sprintf(pathbuf, "%s/%s", getenv("PWD"), STR(json_arr_get(game_data, "textures", i)));
		SDL_Surface *sur = IMG_Load(pathbuf);

		da_push(*ctx->texs, SDL_CreateTextureFromSurface(ctx->rend, sur));
		SDL_FreeSurface(sur);
	}
	cJSON_Delete(game_data);
	return (!!ctx->map);
}

void maze_data_free()
{
	maze_game_context_t *ctx = game_ctx();

	for (size_t i = 0; i < da_count(*ctx->texs); i++)
		SDL_DestroyTexture(da_get(*ctx->texs, i));
	da_free(*ctx->texs);
	map_free(ctx->map);
}
