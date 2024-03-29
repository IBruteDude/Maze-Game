#include "maze_base.h"
#include "maze_map.h"

#include "maze_render.h"
#include "maze_data.h"

#include "maze_events.h"

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	maze_game_context_t *ctx = game_ctx();
	player_t *pl = ctx->pl;
	struct timespec tlast = {0}, tnow = {0};
	double delay;
	bool capfps = false;

	ctx->raycaster = 3;
	ctx->hoff = ctx->voff = 0;

	SDL_SetRelativeMouseMode(true);

	if (!maze_data_load("assets/save.json")) {
		fprintf(stderr, "error loading game data\n");
		exit(EXIT_FAILURE);
	}

	timespec_get(&tlast, TIME_UTC);
	while (true)
	{
		maze_event_handler(ctx);

		SDL_RenderClear(ctx->rend);


/*
TODO

extract little functional dependancies
object collision detector
texturer
state machine

*/

		switch (ctx->raycaster)
		{
		case 0:
			first_renderer_prototype(ctx->map, pl);
			break;
		case 1:
			tutorial_renderer(ctx->map, pl);
			break;
		case 2:
			raycaster(ctx->map, pl);
			break;
		case 3:
			raycaster_2D_preview(ctx->map, pl);
			break;
		}

		timespec_get(&tnow, TIME_UTC);

		ctx->dt = (tnow.tv_sec - tlast.tv_sec) +
			1e-9 * (tnow.tv_nsec - tlast.tv_nsec);
		tlast = tnow;
		if (capfps)
		{
			delay = ctx->dtmin - ctx->dt;
			if (delay > 0)
				SDL_Delay(1000 * delay);
		}

		SDL_RenderPresent(ctx->rend);

		pl->x += pl->xvel;
		pl->y += pl->yvel;
		if (pl->x >= ctx->map->w - 1 || pl->x <= 0 ||
			map_get(ctx->map, pl->x, pl->y) != FLOOR)
			pl->x -= pl->xvel;
		if (pl->y >= ctx->map->h - 1 || pl->y <= 0 ||
			map_get(ctx->map, pl->x, pl->y) != FLOOR)
			pl->y -= pl->yvel;
	}
	return (EXIT_SUCCESS);
}
