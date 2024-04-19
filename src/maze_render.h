#ifndef MAZE_RENDER_H
#define MAZE_RENDER_H

#include "maze_base.h"
#include "maze_ui.h"
#include "olc.h"

#define FOV_W (60 * PI / 180)
#define FOV_H (FOV_W * WIN_H / WIN_W)

#define WALL_H 1.f

void draw_texture_line(double xs, double ys, double xe, double ye, double t_param, SDL_Texture *tex, double cap)
{
	maze_game_context_t *ctx = game_ctx();
	int w, h;

	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	if (cap <= 0)
		cap = w;
	if (xs == xe)
	{
		SDL_Rect srcrect = { .x = t_param * w, .y = 0, .w = MAX(1.0, ceil(w/cap)), .h = h };
		SDL_Rect dstrect = { .x = xs, .y = ys, .w = 1, .h = (ye - ys) };

		SDL_RenderCopy(ctx->rend, tex, &srcrect, &dstrect);
	}
	else
	{
		SDL_Rect srcrect = { .x = t_param * w, .y = 0, .w = MAX(1.0, ceil(w/cap)), .h = h };
		double angle = atan2(ye - ys, xe - xs) * (180.0 / PI);
		SDL_FPoint center = { .x = 0.5, .y = 0.5 };
    	SDL_FRect dstrect = { .x = xs, .y = ys, .w = sqrt((xe - xs) * (xe - xs) + (ye - ys) * (ye - ys)), .h = 1 };
    	
    	SDL_RenderCopyExF(ctx->rend, tex, &srcrect, &dstrect, angle, &center, SDL_FLIP_NONE);
	}
}

void calculate_line_coords(double px, double py, double pview,
	double x, double y, double wx[2], double wy[2])
{
	double	dy[2] = { x - px, x - px},
			dx[2] = { y - py, y - py + 1},
			dz = WALL_H/2, theta[2], phi[2], hypo;

	for (int i = 0; i < 2; i++)
	{
		theta[i] = (atan2(dx[i], dy[i]) + FOV_W/2) - pview;
		hypo = sqrt(dx[i] * dx[i] + dy[i] * dy[i] + dz * dz);
		phi[i] = asin(dz / hypo) + FOV_H/2;
		CLAMP_ANGLE(theta[i]);
		CLAMP_ANGLE(phi[i]);
		wx[i] = theta[i] / FOV_W * WIN_W;
		wy[i] = phi[i] / FOV_H * WIN_H;
	}
}

void render_map_tile(map_t *map, player_t *pl, int i, int j)
{
	double x = pl->x, y = pl->y;
	double wx[4], wy[4], dists[2][2];
	maze_game_context_t *ctx = game_ctx();


	calculate_line_coords(x, y, pl->view, i, j, wx, wy);
	calculate_line_coords(x, y, pl->view, i + 1, j, wx + 2, wy + 2);

	dists[0][0] = wx[2] - wx[0], dists[0][1] = wy[2] - wy[0],
	dists[1][0] = wx[3] - wx[1], dists[1][1] = wy[3] - wy[1];

	dists[0][0] *= dists[0][0], dists[0][1] *= dists[0][1],
	dists[1][0] *= dists[1][0], dists[1][1] *= dists[1][1];

	dists[0][0] += dists[0][1];
	dists[1][0] += dists[1][1];

	dists[0][0] = sqrt(MAX(dists[0][0], dists[1][0]));

	SDL_Color tile_color = {
		(i * 50 + j * 60) & 255,
		(i * 80 + j * 90) & 255,
		(i * 110 + j * 120) & 255,
		255};
	
	if (ctx->textured)
		tile_color.r = tile_color.g = tile_color.b = tile_color.a = 255;

	TRANSFORM4(wy, -= WIN_H / 70);

#ifndef NO_FLOAT_RENDERING
	SDL_Vertex verts[4] = {
		{ .position = { .x = wx[0], .y = wy[0]}, .color = tile_color, .tex_coord = { .x = 0.0, .y = 0.0 } },
		{ .position = { .x = wx[1], .y = wy[1]}, .color = tile_color, .tex_coord = { .x = 0.0, .y = 1.0 } },
		{ .position = { .x = wx[2], .y = wy[2]}, .color = tile_color, .tex_coord = { .x = 1.0, .y = 0.0 } },
		{ .position = { .x = wx[3], .y = wy[3]}, .color = tile_color, .tex_coord = { .x = 1.0, .y = 1.0 } }
	};
	int indicies[6] = {
		0, 2, 3,
		0, 3, 1
	};

	switch (map_get(map, i, j))
	{
	case MAP_FLOOR:
		SDL_RenderGeometry(ctx->rend, (ctx->textured) ? ctx->texs->p[1] : NULL, &verts[0], 4, indicies, 6);
#define FLIP_VERTICALLY(x) (x).position.y = (WIN_H - (x).position.y)
		APPLY4(verts, FLIP_VERTICALLY);
		SDL_RenderGeometry(ctx->rend, (ctx->textured) ? ctx->texs->p[2] : NULL, &verts[0], 4, indicies, 6);
		break;
	default:
		break;
	}
#else
#define CAP (dists[0][0])
	SDL_SetRenderDrawColor(ctx->rend, CSS_RGBA(tile_color));
	for (int k = 0; k < CAP; k++)
	{
		calculate_line_coords(x, y, pl->view, i + k / (CAP), j, wx, wy);
		wy[0] -= WIN_H / 70;
		wy[1] -= WIN_H / 70;
		switch (map_get(map, i, j))
		{
		case MAP_FLOOR:
			if (ctx->textured)
				draw_texture_line(wx[0], wy[0], wx[1], wy[1], k / (CAP), ctx->texs->p[1], CAP),
				draw_texture_line(wx[0], WIN_H - wy[0], wx[1], WIN_H - wy[1], k / (CAP), ctx->texs->p[2], CAP);
			else
				SDL_RenderDrawLineF(ctx->rend, wx[0], wy[0], wx[1], wy[1]),
				SDL_RenderDrawLineF(ctx->rend, wx[0], WIN_H - wy[0], wx[1], WIN_H - wy[1]);
			break;
		default:
			break;
		}
	}
#endif
}

void raycaster_renderer(map_t *map, player_t *pl)
{
	maze_game_context_t *ctx = game_ctx();
	int w = map->w, h = map->h, dirx = SIGN(cos(pl->view)), diry = SIGN(sin(pl->view));
	double tex_lerp_param, lineh, px_angle_dt = FOV_W / WIN_W,
		tan_th, itan_th, tan_fh2 = tan(FOV_H / 2) * 2;
	static double dresults[WIN_W][3];
	static bool bresults[WIN_W];

	ctx->pl_viewed[(int)pl->x][(int)pl->y] = 1;
	if (pl->x + 1 < map->w)
		ctx->pl_viewed[(int)pl->x + 1][(int)pl->y] = 1;
	if (0 <= pl->x - 1)
		ctx->pl_viewed[(int)pl->x - 1][(int)pl->y] = 1;
	if (pl->y + 1 < map->h)
		ctx->pl_viewed[(int)pl->x][(int)pl->y + 1] = 1;
	if (0 <= pl->y - 1)
		ctx->pl_viewed[(int)pl->x][(int)pl->y - 1] = 1;

	for (int i = -(WIN_W / 2); i < WIN_W / 2; i++)
		olc_distance(map, pl->x, pl->y, pl->view + i * px_angle_dt,
						&dresults[i + WIN_W/2][0],
						&dresults[i + WIN_W/2][1],
						&dresults[i + WIN_W/2][2],
						&bresults[i + WIN_W/2]);

	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++)
			if (map_get(map, i, j) == MAP_FLOOR && ctx->pl_viewed[i][j])
				render_map_tile(map, pl, i, j), ctx->pl_viewed[i][j] = 0;

	for (int i = -(WIN_W / 2); i < WIN_W / 2; i++)
	{
		double d = dresults[i + WIN_W/2][0];
		double hd = dresults[i + WIN_W/2][1];
		double vd = dresults[i + WIN_W/2][2];
		bool north_or_south = bresults[i + WIN_W/2];

		lineh = d * tan_fh2;

		if (ctx->textured)
		{
			tan_th = tan(pl->view + i * px_angle_dt), itan_th = 1 / tan_th;
			tex_lerp_param = (north_or_south) ?
				(vd * itan_th + pl->x) : (hd * tan_th + pl->y);
			tex_lerp_param -= floor(tex_lerp_param);
			int tex;
			switch (map_get(map, pl->x + hd + dirx * EPSILON, pl->y + vd + diry * EPSILON))
			{
				case MAP_ENTERANCE: case MAP_EXIT: tex = 3;
					break;
				default: tex = 0;
			}
			draw_texture_line(
				i + WIN_W/2, WIN_H * (0.5 - WALL_H / (2 * lineh)),
				i + WIN_W/2, WIN_H * (0.5 + WALL_H / (2 * lineh)),
				tex_lerp_param, ctx->texs->p[tex], -1);
		}
		else
		{
			if (north_or_south)
				SDL_SetRenderDrawColor(ctx->rend, 255, 255, 255, 255);
			else
				SDL_SetRenderDrawColor(ctx->rend, 127, 127, 127, 255);
			SDL_RenderDrawLine(ctx->rend,
				i + WIN_W/2, WIN_H * (0.5 - WALL_H / (2 * lineh)),
				i + WIN_W/2, WIN_H * (0.5 + WALL_H / (2 * lineh)));
		}
	}

	if (ctx->fpsdisplay)
	{
		char fpsbuf[100];
		snprintf(fpsbuf, 100, "fps: %.2f", 1 / ctx->dt);
		render_text(fpsbuf, 0, 0, ctx->fz);
	}

	if (ctx->helpmsg)
		help_message();

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
			switch (map_get(map, j, i))
			{
			case MAP_EXIT: case MAP_WALL:
				SDL_SetRenderDrawColor(ctx->rend, 255, 255, 255, 255);
				break;
			case MAP_FLOOR:
				SDL_SetRenderDrawColor(ctx->rend, 100, 100, 100, 255);
				break;
			case MAP_ENTERANCE:
				SDL_SetRenderDrawColor(ctx->rend, 100, 255, 255, 255);
				break;
			}
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
	double d, hd, vd, px_angle_dt = FOV_W / WIN_W;
	bool north_or_south;

	CLAMP_ANGLE(pl->view);
	for (int ray_n = -(WIN_W / 2); ray_n < WIN_W / 2; ray_n+=8) {
		double  x = pl->x, y = pl->y, theta = pl->view + ray_n * px_angle_dt,
				tan_th = tan(theta), itan_th = 1 / tan_th;

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

	if (ctx->fpsdisplay)
	{
		char fpsbuf[100];
		snprintf(fpsbuf, 100, "fps: %.2f", 1 / ctx->dt);
		render_text(fpsbuf, 0, 0, ctx->fz);
	}
	{
		char fpsbuf[100];
#define DISPLAY(buf, x) \
		snprintf(buf, 100, #x ": %.2f", (double)x); \
		render_text(buf, ((WIN_W - MAPSIZE)/2 + MAPSIZE)/(double)WIN_W, __COUNTER__ * ctx->fz / (double)WIN_H, ctx->fz/1.5);

		double x = pl->x, y = pl->y, view_angle = pl->view * 180/PI;
		DISPLAY(fpsbuf, x);
		DISPLAY(fpsbuf, y);
		DISPLAY(fpsbuf, view_angle);
	}
	SDL_SetRenderDrawColor(ctx->rend, 0, 0, 0, 255);
}


#endif
