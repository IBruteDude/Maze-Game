#include "maze_base.h"
#include "maze_map.h"

#include "maze_render.h"
#include "maze_data.h"

#include "maze_events.h"

int main(int argc, char *argv[])
{
	(void)argc, (void)argv;
	maze_game_context_t *ctx = game_ctx();
	player_t *pl = ctx->pl;

	SDL_SetRelativeMouseMode(true);
	if (!maze_data_load("assets/save.json")) {
		fprintf(stderr, "error loading game data\n");
		exit(EXIT_FAILURE);
	}
	timespec_get(ctx->tlast, TIME_UTC);
	while (true)
	{
		maze_event_handler(ctx);

		SDL_RenderClear(ctx->rend);
		switch (ctx->state)
		{
		case 0:
			tutorial_renderer(ctx->map, pl);
			break;
		case 1:
			raycaster_textured(ctx->map, pl);
			break;
		case 2:
			raycaster(ctx->map, pl);
			break;
		case 3:
			raycaster_2D_preview(ctx->map, pl);
			break;
		}
		calculate_time_tick();
		SDL_RenderPresent(ctx->rend);
		calculate_player_tick();
	}
	return (EXIT_SUCCESS);
}
