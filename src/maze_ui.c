#include "maze_ui.h"

#define SCALEFACTOR (WIN_W/500.0)

void render_text(const char *message, double winx, double winy, int size)
{
	maze_game_context_t *ctx = game_ctx();
	int len = strlen(message), font_size = ctx->fz;
	int x = winx * WIN_W, y = winy * WIN_H;
	
	if (size == -1)
		size = font_size;
	for (int i = 0; i < len; i++)
	{
		SDL_Texture* char_tex = ctx->char_texs[(int)message[i]];
		int w, h;
		SDL_QueryTexture(char_tex, NULL, NULL, &w, &h);

		if (message[i] == '\n')
		{
			y += size/2.4, x = winx * WIN_W;
			continue;
		}

		SDL_Rect textbox = {
			.x = x, .y = y,
			.h = (double)h * SCALEFACTOR * size / WIN_W,
			.w = (double)w * SCALEFACTOR * size / WIN_W
		};

		SDL_RenderCopy(ctx->rend, char_tex, NULL, &textbox);
		x += (double)w * SCALEFACTOR * size / WIN_W;
		if (x > WIN_W)
			y += size/2.4, x = winx * WIN_W;
	}
}

/**
 * help_message - display a help message containing the options for the player
 */
void help_message(void)
{
	maze_game_context_t *ctx = game_ctx();
	SDL_Rect outer_r = { .x = 0.05 * WIN_W, .y = 0.05 * WIN_H, .w = 0.90 * WIN_W, .h = 0.90 * WIN_H };
	SDL_Rect inner_r = { .x = 0.055 * WIN_W, .y = 0.055 * WIN_H, .w = 0.89 * WIN_W, .h = 0.89 * WIN_H };
	int font_size = ctx->fz;
	const char options[] =  "- Press W / UP ARROW to go forward\n\n"
							"- Press S / DOWN ARROW to go backward\n\n"
							"- Press A / LEFT ARROW to go left\n\n"
							"- Press D / RIGHT ARROW to go right\n\n"
							"- Hold SHIFT to sprint\n\n"
							"- Press T to toggle textures\n\n"
							"- Press M to toggle minimap\n\n"
							"- Press ESCAPE to release the mouse\n\n"
							"- Click to focus and move the camera\n\n";

	SDL_SetRenderDrawColor(ctx->rend, CSS_RGBA(RGB_white));
	SDL_RenderFillRect(ctx->rend, &outer_r);
	SDL_SetRenderDrawColor(ctx->rend, CSS_RGBA(RGB_chocolate));
	SDL_RenderFillRect(ctx->rend, &inner_r);
	render_text("Options:", 0.10, 0.10, font_size * 3);
	render_text(options, 0.20, 0.30, font_size * 2/3);
	render_text("Press H to toggle this message", 0.10, 0.85, font_size * 2/3);
}

/**
 * main_screen - display the game's main screen
 */
void main_screen(void)
{
	maze_game_context_t *ctx = game_ctx();
	static SDL_Texture *maze_logo;

	if (maze_logo == NULL)
	{
		char pathbuf[PATH_MAX];
		sprintf(pathbuf, "%s/assets/baked/images/%s.bmp", getenv("PWD"), "logo");
		
		SDL_Surface *sur = SDL_LoadBMP(pathbuf);

		maze_logo = SDL_CreateTextureFromSurface(ctx->rend, sur);
		SDL_FreeSurface(sur);
	}
	int w, h;
	SDL_QueryTexture(maze_logo, NULL, NULL, &w, &h);

	w *= 1.75;
	h *= 1.75;

	SDL_Rect r = {
		.x = (WIN_W - w)/2, .y = 0.25 * WIN_H,
		.w = w, .h = h,
	};

	SDL_RenderCopy(ctx->rend, maze_logo, NULL, &r);

	render_text("Press Enter to Start", 0.12, 0.7, ctx->fz * 2);

	SDL_SetRenderDrawColor(ctx->rend, CSS_RGBA(RGB_chocolate));
}

/**
 * win_screen - display the player winning screen
 */
void win_screen(void)
{
	maze_game_context_t *ctx = game_ctx();
	char timebuf[30];

	render_text("Congratulations", 0.02, 0.35, ctx->fz * 3);
	render_text("You escaped the Maze!", 0.1, 0.5, ctx->fz * 2);
	sprintf(timebuf, "Round time: %.2f secs",
		ctx->rtend.tv_sec - ctx->rtstart.tv_sec + ((ctx->rtend.tv_nsec - ctx->rtstart.tv_nsec) / 1e9)
	);
	render_text(timebuf, 0.31, 0.7, ctx->fz);
	render_text("Press Enter to Go Back", 0.3, 0.8, ctx->fz);

	SDL_SetRenderDrawColor(ctx->rend, CSS_RGBA(RGB_chocolate));
}
