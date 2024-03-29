#include "maze_events.h"
#include "maze_base.h"


#define PLAYER_SPEED 0.3

int maze_event_handler(void)
{
	maze_game_context_t *ctx = game_ctx();
	SDL_Event event;
	static double xdisplacement, ydisplacement, rotation;

	while (SDL_PollEvent(&event))
	{
		rotation = 0.0;
		switch (event.type)
		{
		case SDL_QUIT: exit(EXIT_SUCCESS);
		case SDL_MOUSEBUTTONDOWN:
		{
			SDL_SetRelativeMouseMode(1);
			ctx->raycaster = (ctx->raycaster + event.button.button) & 3;
			break;
		}
		

		case SDL_MOUSEMOTION:
			// printf("view before: %f", ctx->pl->view);
			ctx->pl->view += ((double)event.motion.xrel / WIN_W) * 2 * PI;
			CLAMP_ANGLE(ctx->pl->view);
			// printf("view after: %f", ctx->pl->view);
			// printf("mouse event: xrel %d\n", event.motion.xrel);
			// DEBUG2F(ctx->pl->view, event.motion.xrel);
			break;
		case SDL_KEYDOWN:
			// printf("pressed: %d", event.key.keysym.sym);
			switch (event.key.keysym.sym)
			{
				case SDLK_a: case SDLK_LEFT:
					rotation += PI/2;
					// ctx->pl->x += PLAYER_SPEED * cos(ctx->pl->view - 90);
					// ctx->pl->y += PLAYER_SPEED * sin(ctx->pl->view - 90);
					// ctx->pl->y += PLAYER_SPEED * cos(ctx->pl->view);
					// ctx->pl->x += PLAYER_SPEED * sin(ctx->pl->view);
					// break;
				case SDLK_s: case SDLK_DOWN:
					rotation += PI/2;
					// ctx->pl->x += PLAYER_SPEED * cos(ctx->pl->view - 90);
					// ctx->pl->x += PLAYER_SPEED * cos(ctx->pl->view + 180);
					// ctx->pl->y += PLAYER_SPEED * sin(ctx->pl->view + 180);
					// ctx->pl->x -= PLAYER_SPEED * cos(ctx->pl->view);
					// ctx->pl->y -= PLAYER_SPEED * sin(ctx->pl->view);
					// break;
				case SDLK_d: case SDLK_RIGHT:
					rotation += PI/2;
					// ctx->pl->x += PLAYER_SPEED * cos(ctx->pl->view - 90);
					// ctx->pl->x += PLAYER_SPEED * cos(ctx->pl->view + 90);
					// ctx->pl->y += PLAYER_SPEED * sin(ctx->pl->view + 90);
					// ctx->pl->y -= PLAYER_SPEED * cos(ctx->pl->view);
					// ctx->pl->x -= PLAYER_SPEED * sin(ctx->pl->view);
					// break;
				case SDLK_w: case SDLK_UP:
					ctx->pl->x += xdisplacement = PLAYER_SPEED * cos(ctx->pl->view + rotation);
					ctx->pl->y += ydisplacement = PLAYER_SPEED * sin(ctx->pl->view + rotation);
					break;
				case SDLK_ESCAPE:
					SDL_SetRelativeMouseMode(0);
					break;
				case SDLK_LSHIFT:
					ctx->raycaster = (ctx->raycaster + 3) & 3;
					break;
				case SDLK_RSHIFT:
					ctx->raycaster = (ctx->raycaster + 1) & 3;
					break;
				case SDLK_v:
					ctx->voff++;
					break;
				case SDLK_b:
					ctx->voff--;
					break;
				case SDLK_h:
					ctx->hoff++;
					break;
				case SDLK_j:
					ctx->hoff--;
					break;
			}
		}

		if (ctx->pl->x >= ctx->map->w - 1 ||
			ctx->pl->x <= 0 ||
			map_get(ctx->map, ctx->pl->x, ctx->pl->y) != FLOOR)
			ctx->pl->x -= xdisplacement;

		if (ctx->pl->y >= ctx->map->h - 1 ||
			ctx->pl->y <= 0 ||
			map_get(ctx->map, ctx->pl->x, ctx->pl->y) != FLOOR)
			ctx->pl->y -= ydisplacement;
	}
	return 1;
}

