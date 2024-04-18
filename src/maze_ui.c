#include "maze_ui.h"

void render_text(const char *message, int x, int y, int size)
{
	maze_game_context_t *ctx = game_ctx();
	int len = strlen(message);
	
	if (size == -1)
		size = ctx->fz;
	for (int i = 0; i < len; i++)
	{
		SDL_Texture* char_tex = ctx->char_texs[(int)message[i]];

		int w, h;
		SDL_QueryTexture(char_tex, NULL, NULL, &w, &h);

		SDL_Rect textbox = {
			.x = x, .y = y,
			.h = (double)h * size / WIN_W,
			.w = (double)w * size / WIN_W
		};

		SDL_RenderCopy(ctx->rend, char_tex, NULL, &textbox);
		x += (double)w * size / WIN_W;		
	}
}
