#include "maze_base.h"

maze_game_context_t *game_ctx(void) {
	static map_t map;
	static player_t pl;
	static maze_resource_stack_t resource_stack;
	static maze_game_context_t global_game_context = {
		.pl = &pl,
		.map = &map,
		.resource_stack = &resource_stack
	};

	da_init(resource_stack);
	return &global_game_context;
}

void CTOR maze_init(void)
{
	maze_game_context_t *ctx = game_ctx();

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		maze_loge("SDL_Init"), exit(EXIT_FAILURE);

	if (TTF_Init() != 0)
		maze_loge("TTF_Init"), exit(EXIT_FAILURE);

	ctx->win = SDL_CreateWindow("Hello World SDL",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WIN_W, WIN_H, SDL_WINDOW_SHOWN);
	if (ctx->win == NULL)
		maze_loge("SDL_CreateWindow"), exit(EXIT_FAILURE);

	ctx->rend = SDL_CreateRenderer(ctx->win, -1,
		SDL_RENDERER_ACCELERATED);
	if (ctx->rend == NULL)
		maze_loge("SDL_Createrend"), exit(EXIT_FAILURE);
}


void DTOR maze_cleanup(void)
{
	maze_game_context_t *ctx = game_ctx();

	while (!da_empty(*ctx->resource_stack)) {
		maze_resource_t r = da_pop(*ctx->resource_stack);

		if (r.resource != NULL)
			r.deallocator(r.resource);
	}

	if (ctx->win != NULL)
		SDL_DestroyWindow(ctx->win);
	if (ctx->rend != NULL)
		SDL_DestroyRenderer(ctx->rend);

	TTF_Quit();
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
