#include "maze_render.h"
#include "maze_data.h"
#include "maze_events.h"
#include "maze_ui.h"

int main(int argc, char *argv[])
{
	(void)argc, (void)argv;
	/* Get the main game context */
	maze_game_context_t *ctx = game_ctx();
	player_t *pl = ctx->pl;

	/* Load all the game data from the config.json file */
	if (!maze_data_load("assets/config.json"))
	{
		fprintf(stderr, "error loading game data\n");
		exit(EXIT_FAILURE);
	}
	/* Initialise last render frame time */
	timespec_get(ctx->tlast, TIME_UTC);
	while (true)
	{
		/* Handle any user-triggered events */
		maze_event_handler(ctx);

		/* Render the current state of the game */
		SDL_RenderClear(ctx->rend);
		switch (ctx->state)
		{
		case MAZE_3D:
			raycaster_renderer(ctx->map, pl);
			break;
		case MAZE_MINIMAP:
			raycaster_2D_preview(ctx->map, pl);
			break;
		case MAZE_MAINSCREEN:
			main_screen();
			break;
		case MAZE_WINSCREEN:
			win_screen();
			break;
		}
		/* Calculate current frame time and delta-time */
		calculate_time_tick();
		SDL_RenderPresent(ctx->rend);
	}
	return (EXIT_SUCCESS);
}
