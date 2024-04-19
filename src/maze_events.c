#include "maze_events.h"
#include "maze_base.h"

#define SENSITIVITY 0.5


void handle_screen_change(maze_game_context_t *ctx, SDL_Event *event)
{
	player_t *pl = ctx->pl;

	if (event->type != SDL_KEYDOWN)
		return ;
	if (ctx->state == MAZE_MAINSCREEN)
		ctx->state = MAZE_3D, timespec_get(&ctx->rtstart, TIME_UTC), printf("called start\n");
	else if (ctx->state == MAZE_WINSCREEN)
	{
		int sx = ctx->map->start_x, sy = ctx->map->start_y;

		if (map_get(ctx->map, sx + 1, sy) == MAP_FLOOR)
			sx++;
		else if (map_get(ctx->map, sx, sy + 1) == MAP_FLOOR)
			sy++;
		ctx->state = MAZE_MAINSCREEN, ctx->helpmsg = true,
		pl->x = sx + 0.5, pl->y = sy + 0.5;
	}
}

void handle_key(maze_game_context_t *ctx, SDL_Event *event)
{
	player_t *pl = ctx->pl;

	switch (event->key.keysym.sym)
	{
		case SDLK_a: case SDLK_LEFT:	pl->xvel = (event->type == SDL_KEYDOWN) ? -1 : 0;	break;
		case SDLK_s: case SDLK_DOWN:	pl->yvel = (event->type == SDL_KEYDOWN) ? -1 : 0;	break;
		case SDLK_d: case SDLK_RIGHT:	pl->xvel = (event->type == SDL_KEYDOWN) ?  1 : 0;	break;
		case SDLK_w: case SDLK_UP:		pl->yvel = (event->type == SDL_KEYDOWN) ?  1 : 0;	break;
		case SDLK_ESCAPE:
			SDL_SetRelativeMouseMode(0);
			ctx->focused = 0;
			break;
		case SDLK_LSHIFT: case SDLK_RSHIFT:
			if (event->type == SDL_KEYDOWN && (pl->yvel == 1))
				pl->yvel = 2;
			else if (event->type == SDL_KEYUP && (pl->yvel == 2))
				pl->yvel = 1;
			break;
		case SDLK_m:
			if (event->type == SDL_KEYDOWN)
			{
				if (ctx->state == MAZE_3D)
					ctx->state = MAZE_MINIMAP;
				else if (ctx->state == MAZE_MINIMAP)
					ctx->state = MAZE_3D;
			}
			break;
		case SDLK_t:
			if (event->type == SDL_KEYDOWN)
				ctx->textured = !ctx->textured;
			break;
		case SDLK_f:
			if (event->type == SDL_KEYDOWN && ctx->state == MAZE_3D)
				ctx->fpsdisplay = !ctx->fpsdisplay;
			break;
		case SDLK_RETURN:
			handle_screen_change(ctx, event);
			break;
		case SDLK_v:
			ctx->voff++;
			break;
		case SDLK_b:
			ctx->voff--;
			break;
		case SDLK_h:
			if (event->type == SDL_KEYDOWN)
				ctx->helpmsg = !ctx->helpmsg;
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
		ctx->focused = 1;
		break;
	}
	case SDL_MOUSEMOTION:
		if (ctx->focused) {
			ctx->pl->view += SENSITIVITY * ((double)event->motion.xrel / WIN_W) * 2 * PI;
			CLAMP_ANGLE(ctx->pl->view);
		}
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
	handle_player_movement();
	return (NULL);
}

void calculate_time_tick(void)
{
	maze_game_context_t *ctx = game_ctx();
	struct timespec tnow;

	timespec_get(&tnow, TIME_UTC);

	ctx->dt = (tnow.tv_sec - ctx->tlast->tv_sec) +
			1e-9 * (tnow.tv_nsec - ctx->tlast->tv_nsec);
	*ctx->tlast = tnow;
	if (ctx->capfps)
	{
		double delay = ctx->dtmin - ctx->dt;
		if (delay > 0)
			SDL_Delay(1000 * delay);
	}
}

void handle_player_movement(void)
{
	maze_game_context_t *ctx = game_ctx();
	player_t *pl = ctx->pl;
	double newx, newy, dx, dy, dxx, dxy, dyx, dyy;

	if (ctx->state == MAZE_MAINSCREEN || ctx->state == MAZE_WINSCREEN)
		return;
	dxx = pl->xvel * cos(pl->view + PI/2);
	dxy = pl->xvel * sin(pl->view + PI/2);
	dyx = pl->yvel * cos(pl->view);
	dyy = pl->yvel * sin(pl->view);
	newx = pl->x + (dx = ctx->dt * pl->speed * (dxx + dyx));
	newy = pl->y + (dy = ctx->dt * pl->speed * (dxy + dyy));


	if (0 < newx && newx < ctx->map->w)
		switch (map_get(ctx->map, newx, pl->y))
		{
		case MAP_EXIT:
			ctx->state = MAZE_WINSCREEN;
			timespec_get(&ctx->rtend, TIME_UTC);
			break;	
		case MAP_FLOOR:
			pl->x = newx;
			break;
		default:
			break;
		}
	if (0 < newy && newy < ctx->map->h)
		switch (map_get(ctx->map, pl->x, newy))
		{
		case MAP_EXIT:
			ctx->state = MAZE_WINSCREEN;
			timespec_get(&ctx->rtend, TIME_UTC);
			break;	
		case MAP_FLOOR:
			pl->y = newy;
			break;
		default:
			break;
		}
}
