#include "maze_base.h"
#include "maze_data.h"

/**
 */
maze_game_context_t *game_ctx(void)
{
	static map_t map;
	static player_t pl;
	static maze_resource_stack_t resource_stack;
	static texture_pack_t texture_pack;
	static struct timespec last_frame_time;
	static maze_game_context_t global_game_context = {
		.pl = &pl,
		.map = &map,
		.resource_stack = &resource_stack,
		.texs = &texture_pack,
		.tlast = &last_frame_time,
		.state = MAZE_MAINSCREEN,
		.hoff = 0,
		.voff = 0,
		.textured = true,
		.focused = true,
		.helpmsg = true,
		.fpsdisplay = true,
	};

	return &global_game_context;
}

void CTOR maze_init(void)
{
	maze_game_context_t *ctx = game_ctx();
	char pathbuf[PATH_MAX];
	
	sprintf(pathbuf, "%s/%s", getenv("PWD"), "logs/output.log");
	freopen(pathbuf, "w", stdout);
	sprintf(pathbuf, "%s/%s", getenv("PWD"), "logs/error.log");
	freopen(pathbuf, "w", stderr);

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		maze_loge("SDL_Init"), exit(EXIT_FAILURE);


	ctx->win = SDL_CreateWindow("Maze Game",
								SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
								WIN_W, WIN_H, SDL_WINDOW_SHOWN);
	if (ctx->win == NULL)
		maze_loge("SDL_CreateWindow"), exit(EXIT_FAILURE);

	ctx->rend = SDL_CreateRenderer(ctx->win, -1,
								   SDL_RENDERER_ACCELERATED);
	if (ctx->rend == NULL)
		maze_loge("SDL_CreateRenderer"), exit(EXIT_FAILURE);

	da_init(*ctx->resource_stack);
	da_init(*ctx->texs);
}


void DTOR maze_cleanup(void)
{
	maze_game_context_t *ctx = game_ctx();

	maze_data_free();
	while (!da_empty(*ctx->resource_stack)) {
		maze_resource_t r = da_pop(*ctx->resource_stack);

		if (r.resource != NULL)
			r.deallocator(r.resource);
	}

	da_free(*ctx->resource_stack);
	da_free(*ctx->texs);

	if (ctx->win != NULL)
		SDL_DestroyWindow(ctx->win);
	if (ctx->rend != NULL)
		SDL_DestroyRenderer(ctx->rend);

	/** Weird bug with SDL_Quit **/
	/* SDL_Quit(); */
}

bool maze_resource_stack_push(void *resource, void (*deallocator)(void *))
{
	maze_game_context_t *ctx = game_ctx();

	maze_resource_t r = { .resource = resource, .deallocator = deallocator };
	return da_push(*ctx->resource_stack, r);
}

void maze_resource_stack_pop(void)
{
	maze_game_context_t *ctx = game_ctx();
	maze_resource_t r = da_pop(*ctx->resource_stack);

	if (r.resource)
		r.deallocator(r.resource);
}
