#include "maze_events.h"
#include "maze_base.h"


void handle_key(maze_game_context_t *ctx, SDL_Event *event)
{
	double rotation;
	player_t *pl = ctx->pl;

	rotation = 0.0;
	switch (event->key.keysym.sym)
	{
		case SDLK_a: case SDLK_LEFT: rotation += PI/2;
		case SDLK_s: case SDLK_DOWN: rotation += PI/2;
		case SDLK_d: case SDLK_RIGHT: rotation += PI/2;
		case SDLK_w: case SDLK_UP:
			pl->xvel = (event->type == SDL_KEYDOWN) ? ctx->dt * pl->speed * cos(pl->view + rotation) : 0;
			pl->yvel = (event->type == SDL_KEYDOWN) ? ctx->dt * pl->speed * sin(pl->view + rotation) : 0;
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
void handle_mouse(maze_game_context_t *ctx, SDL_Event *event)
{
	switch (event->type)
	{
	case SDL_MOUSEBUTTONDOWN:
	{
		SDL_SetRelativeMouseMode(1);
		ctx->raycaster = (ctx->raycaster + event->button.button) & 3;
		break;
	}
	case SDL_MOUSEMOTION:
		ctx->pl->view += ((double)event->motion.xrel / WIN_W) * 2 * PI;
		CLAMP_ANGLE(ctx->pl->view);
		// DEBUG2F(ctx->pl->view, event.motion.xrel);
		break;
	}
}

void *maze_event_handler(void *data)
{
	maze_game_context_t *ctx = data;
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT: exit(EXIT_SUCCESS);
		case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEMOTION:
			handle_mouse(ctx, &event);
			break;
		case SDL_KEYDOWN: case SDL_KEYUP:
			handle_key(ctx, &event);
			break;
		}
	}
	return (NULL);
}

