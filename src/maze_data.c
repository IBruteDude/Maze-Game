#include "maze_data.h"

bool maze_data_load(const char *savefile)
{
	maze_game_context_t *ctx = game_ctx();
	cJSON *game_data, *player;
	char *buf, pathbuf[PATH_MAX] = "";

	sprintf(pathbuf, "%s/%s", getenv("PWD"), savefile);
	buf = loadfile(pathbuf), game_data = cJSON_Parse(buf), free(buf);
	if (!game_data)
		return (false);

	player = JOBJ(game_data, "player");
	ctx->dtmin = 1 / json_num(game_data, "fpsmax"), ctx->dt = ctx->dtmin;
	ctx->capfps = json_bool(game_data, "capfps");
	ctx->pl->x = json_num(player, "x"), ctx->pl->y = json_num(player, "y"),
	ctx->pl->view = PI/180 * json_num(player, "view");
	ctx->pl->speed = json_num(player, "speed");
	ctx->pl->xvel = ctx->pl->yvel = 0;

	map_load(ctx->map, json_str(game_data, "map"));

	ctx->fz = json_num(game_data, "font_size");

	strncpy(ctx->font, json_str(game_data, "font"), 100);

	int tex_num = json_arr_size(game_data, "textures");

	for (int i = 0; i < tex_num; i++)
	{
		sprintf(pathbuf, "%s/%s", getenv("PWD"), JSTR(json_arr_get(game_data, "textures", i)));
		SDL_Surface *sur = SDL_LoadBMP(pathbuf);

		da_push(*ctx->texs, SDL_CreateTextureFromSurface(ctx->rend, sur));
		SDL_FreeSurface(sur);
	}
	cJSON_Delete(game_data);

	ctx->pl_viewed = (bool **)calloc(ctx->map->w, sizeof(bool *));
	for (int i = 0; i < ctx->map->w; i++)
		ctx->pl_viewed[i] = (bool *)calloc(ctx->map->h, sizeof(bool));

	for (char c = 1; c > 0; c++)
	{
		static const SDL_Color White = { 255, 255, 255, 255 };
		char pathbuf[PATH_MAX];
		sprintf(pathbuf, "%s/assets/baked/fonts/%s/%d.bmp", getenv("PWD"), ctx->font, (int)c);
		SDL_Surface *char_sur = SDL_LoadBMP(pathbuf);
		if (!char_sur)
			maze_loge("SDL_LoadBMP");
		ctx->char_texs[(int)c] = SDL_CreateTextureFromSurface(ctx->rend, char_sur);
		SDL_FreeSurface(char_sur);
	}

	return (!!ctx->map);
}

void maze_data_free()
{
	maze_game_context_t *ctx = game_ctx();

	for (char c = 1; c > 0; c++)
		SDL_DestroyTexture(ctx->char_texs[(int)c]);

	for (int i = 0; i < ctx->map->w; i++)
		free(ctx->pl_viewed[i]);
	free(ctx->pl_viewed);

	for (size_t i = 0; i < da_count(*ctx->texs); i++)
		SDL_DestroyTexture(da_get(*ctx->texs, i));
	da_free(*ctx->texs);
	map_free(ctx->map);
}
