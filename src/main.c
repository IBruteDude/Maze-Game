#include "maze_base.h"
#include "maze_map.h"


#include "cjson/cJSON.h"
#include "maze_render.h"
#include "maze_data.h"

#include "maze_events.h"


int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	maze_game_context_t *ctx = game_ctx();

	ctx->raycaster = 3;
	ctx->hoff = ctx->voff = 0;

	SDL_SetRelativeMouseMode(true);

	if (!maze_data_load("assets/save.json")) {
		fprintf(stderr, "error loading game data\n");
		exit(EXIT_FAILURE);
	}

	while (true)
	{
		maze_event_handler();

		SDL_RenderClear(ctx->rend);


/*
TODO

fps counter == deltatime integration
extract little functional dependancies
object collistion detector
texturer
state machine

*/

		switch (ctx->raycaster)
		{
		case 0:
			render_fov(ctx->map, ctx->pl);
			break;
		case 1:
			all_vector_raycast_renderer(ctx->map, ctx->pl);
			break;
		case 2:
			my_all_vector_raycast_renderer(ctx->map, ctx->pl);
			break;
		case 3:
			raycasting_2D(ctx->map, ctx->pl);
			break;
		}

		SDL_RenderPresent(ctx->rend);

		SDL_Delay(1000/ctx->fps);
	}
	return (EXIT_SUCCESS);
}
