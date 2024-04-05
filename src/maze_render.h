#ifndef MAZE_RENDER_H
#define MAZE_RENDER_H

#include "maze_base.h"
#include "maze_ui.h"
#include "olc.h"

#define FOV_W (60 * PI / 180)
#define FOV_H (FOV_W * WIN_H / WIN_W)

#define WALL_H 1.f

void draw_texture_line(double xs, double ys, double xe, double ye, double t_param, SDL_Texture *tex)
{
	maze_game_context_t *ctx = game_ctx();
	int w, h;

	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	if (xs == xe)
	{
		SDL_Rect srcrect = { .x = t_param * w, .y = 0, .w = 1, .h = h };
		SDL_Rect dstrect = { .x = xs, .y = ys, .w = 1, .h = (ye - ys) };

		SDL_RenderCopy(ctx->rend, tex, &srcrect, &dstrect);
	}
	else
	{
		SDL_Rect srcrect = { .x = t_param * w, .y = 0, .w = 1, .h = h };
		SDL_FRect dstrect = { .x = xs, .y = ys, .w = 1, .h = sqrt((xe - xs) * (xe - xs) + (ye - ys) * (ye - ys))};
		double angle = atan2(ye - ys, xe - xs);
		SDL_FPoint center = { .x = xs, .y = ys };

		SDL_RenderCopyExF(ctx->rend, tex, &srcrect, &dstrect, angle, &center, SDL_FLIP_NONE);
	}
}

void raycaster_renderer(map_t *map, player_t *pl)
{
	maze_game_context_t *ctx = game_ctx();
	double d, tex_lerp_param, hd, vd, h, px_angle_dt = FOV_W / WIN_W,
		tan_th, itan_th, tan_fh2 = tan(FOV_H / 2) * 2;
	bool north_or_south = false;

	for (int i = -(WIN_W / 2); i < WIN_W / 2; i++) {
		olc_distance(map, pl->x, pl->y, pl->view + i * px_angle_dt,
						&d, &hd, &vd, &north_or_south);
		h = d * tan_fh2;
		if (ctx->textured) {
			tan_th = tan(pl->view + i * px_angle_dt), itan_th = 1 / tan_th;
			tex_lerp_param = (north_or_south) ?
				(vd * itan_th + pl->x) : (hd * tan_th + pl->y);
			tex_lerp_param -= floor(tex_lerp_param);
			draw_texture_line(i + WIN_W/2, WIN_H * (0.5 - WALL_H / (2 * h)),
							  i + WIN_W/2, WIN_H * (0.5 + WALL_H / (2 * h)),
							  tex_lerp_param, ctx->texs->p[north_or_south]);
		}
		else
		{
			if (north_or_south)
				SDL_SetRenderDrawColor(ctx->rend, 255, 255, 255, 255);
			else
				SDL_SetRenderDrawColor(ctx->rend, 127, 127, 127, 255);
			SDL_RenderDrawLine(ctx->rend,
				i + WIN_W/2, WIN_H * (0.5 - WALL_H / (2 * h)),
				i + WIN_W/2, WIN_H * (0.5 + WALL_H / (2 * h)));
		}
	}
	{
		char fpsbuf[100];
		snprintf(fpsbuf, 100, "fps: %.2f", 1 / ctx->dt);
		render_text(fpsbuf, 0, 0);
	}

	SDL_SetRenderDrawColor(ctx->rend, 0, 0, 0, 255);
}


#define LINE_THICKNESS 		1
#define PLAYER_THICKNESS	4
void render_minimap(map_t *map, int x, int y, double maxw, double maxh,
		player_t *pl, double rayx, double rayy)
{
	maze_game_context_t *ctx = game_ctx();
	double w = map->w, h = map->h, wratio = maxw / w, hratio = maxh / h;
	bool fit_to_height = (maxw / w != maxh / h && maxw / w * h > maxh);
	double block_size = fit_to_height ? hratio : wratio;
	int dirx = pl->x > rayx ? -1 : 1, diry = pl->y > rayy ? -1 : 1;

	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++) {
			SDL_Rect r = {
				.x = x + block_size * j + LINE_THICKNESS,
				.y = y + block_size * i + LINE_THICKNESS,
				.w = block_size - 2 * LINE_THICKNESS,
				.h = block_size - 2 * LINE_THICKNESS,
			};
			if (map_get(map, j, i) != FLOOR)
				SDL_SetRenderDrawColor(ctx->rend, 255, 255, 255, 255);
			else
				SDL_SetRenderDrawColor(ctx->rend, 100, 100, 100, 255);
			SDL_RenderDrawRect(ctx->rend, &r);
		}

	SDL_FRect p = {
		.x = x + pl->x * block_size - PLAYER_THICKNESS,
		.y = y + pl->y * block_size - PLAYER_THICKNESS,
		.w = 2 * PLAYER_THICKNESS, .h =  2 * PLAYER_THICKNESS};
	
	SDL_SetRenderDrawColor(ctx->rend, 180, 80, 20, 255);
	SDL_RenderFillRectF(ctx->rend, &p);

	SDL_SetRenderDrawColor(ctx->rend, 255, 50, 30, 255);

	for (int i = -1; i <= 1; i++)
		SDL_RenderDrawLine(ctx->rend,
			x + pl->x * block_size + LINE_THICKNESS - dirx * i,
			y + pl->y * block_size - LINE_THICKNESS + diry * i,
			x + rayx * block_size + LINE_THICKNESS - dirx * i,
			y + rayy * block_size - LINE_THICKNESS + diry * i
		);
}

void raycaster_2D_preview(map_t *map, player_t *pl)
{
	maze_game_context_t *ctx = game_ctx();
	double SDL_UNUSED d, hd, vd, lineh,
		px_angle_dt = FOV_W / WIN_W, tan_fh2 = tan(FOV_H / 2) * 2;
	bool north_or_south;

	CLAMP_ANGLE(pl->view);
	for (int ray_n = -(WIN_W / 2); ray_n < WIN_W / 2; ray_n+=8) {
		double  x = pl->x, y = pl->y, theta = pl->view + ray_n * px_angle_dt,
				w = map->w, h = map->h, tan_th = tan(theta), itan_th = 1 / tan_th;

		olc_distance(map, pl->x, pl->y, pl->view + ray_n * px_angle_dt,
					&d, &hd, &vd, &north_or_south);

#define MAPSIZE 400
		if (ray_n == 0)
		{
			if (north_or_south)
				render_minimap(map, (WIN_W - MAPSIZE)/2, (WIN_H - MAPSIZE)/2,
					MAPSIZE, MAPSIZE, pl, ((vd * itan_th) + x), (vd + y));
			else
				render_minimap(map, (WIN_W - MAPSIZE)/2, (WIN_H - MAPSIZE)/2,
					MAPSIZE, MAPSIZE, pl, (hd + x), ((hd * tan_th) + y));
		}
	}

	{
		char fpsbuf[100];
		snprintf(fpsbuf, 100, "fps: %.2f", 1 / ctx->dt);
		render_text(fpsbuf, 0, 0);
	}
	SDL_SetRenderDrawColor(ctx->rend, 0, 0, 0, 255);
}


#endif
