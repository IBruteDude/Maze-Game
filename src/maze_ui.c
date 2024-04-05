#include "maze_ui.h"

void render_text(const char *message, int x, int y)
{
	maze_game_context_t *ctx = game_ctx();
	static const SDL_Color White = { 255, 255, 255, 255 };

	SDL_Surface* surfaceMessage =
		TTF_RenderText_Solid(ctx->font, message, White);

	SDL_Texture* msg_tex = SDL_CreateTextureFromSurface(ctx->rend, surfaceMessage);

	SDL_Rect textbox = {.x = x, .y = y, .h = ctx->fz / 2,
						.w = ctx->fz / 2 * strlen(message) / 3};


	SDL_RenderCopy(ctx->rend, msg_tex, NULL, &textbox);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(msg_tex);
}
