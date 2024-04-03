#ifndef MAZE_RENDER_H
#define MAZE_RENDER_H

#include "maze_base.h"
#include "raycaster.h"

#define FOV_W (60 * PI / 180)
#define FOV_H (FOV_W * WIN_H / WIN_W)

#define WALL_H 1.f

void render_text(const char *message, int x, int y) {
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

double distance(dvec2 pl, dvec2 wall)
{
	return (sqrt(
		(pl.x - wall.x) * (pl.x - wall.x) +
		(pl.y - wall.y) * (pl.y - wall.y)
	));
}

double distance_to_nearest_wall(map_t *map, player_t *pl)
{
	double  x = pl->x, y = pl->y,
			ix = floor(x), iy = floor(y),
			fx = x - ix, fy = y - ix,
			dx = cos(pl->view), dy = sin(pl->view),
			dirx = (dx >= 0) ? 1 : -1, diry = (dy >= 0) ? 1 : -1,
			stepx = (dx >= 0) ? 1 - fx : fx, stepy = (dy >= 0) ? 1 - fy : fy,
			is_north_or_south;

	is_north_or_south = fabs((stepx / dx) * dy) > fabs(stepy);
	if (is_north_or_south)
		stepx = (stepy / dy) * dx;
	else
		stepy = (stepx / dx) * dy;
	x += dirx * stepx, y += diry * stepy;

	do {
		if (is_north_or_south)
			stepx = (dx >= 0) ? 1 - (x - ix) : (x - ix),
			stepy = 1;
		else
			stepx = 1, stepy = (dy >= 0) ? (1 - (y - iy)) : (y - iy);

		is_north_or_south = fabs((stepx / dx) * dy) > fabs(stepy);

		if (is_north_or_south)
			stepx = (stepy / dy) * dx, iy += diry;
		else
			stepy = (stepx / dx) * dy, ix += dirx;

		x += dirx * stepx, y += diry * stepy;

		if (ix >= map->w || ix < 0)
		{
			ix -= dirx;
			break;
		}
		if (iy >= map->h || iy < 0)
		{
			iy -= diry;
			break;
		}
 	} while (map_get(map, ix, iy) != FLOOR);

	x -= stepx, y -= stepy;

	return (sqrt((x - pl->x) * (x - pl->x) + (y - pl->y) * (y - pl->y)));
}


void first_renderer_prototype(map_t *map, player_t *pl)
{
	maze_game_context_t *ctx = game_ctx();
	double x = pl->x, y = pl->y, /*theta = pl->view, */ d, h,
		px_angle_dt = FOV_W / WIN_W, tan_fh2 = tan(FOV_H / 2) * 2;



	SDL_SetRenderDrawColor(ctx->rend, 255, 255, 255, 255);
	for (int i = -(WIN_W / 2); i < WIN_W / 2; i++) {
		dvec2 wall = find_nearest_wall(map, x, y, pl->view + i * px_angle_dt);
		d = distance(wall, pl->pos);
		h = d * tan_fh2;

		SDL_RenderDrawLine(ctx->rend,
			i + WIN_W/2, WIN_H * (0.5 - WALL_H / (2 * h)),
			i + WIN_W/2, WIN_H * (0.5 + WALL_H / (2 * h)));
	}
	SDL_SetRenderDrawColor(ctx->rend, 255, 127, 0, 255);

	for (int i = 0; i < map->h; i++)
		for (int j = 0; j < map->w; j++)
			if (map_get(map, i, j) == WALL)
				SDL_RenderDrawPoint(ctx->rend, WIN_H / 2 - map->h + i, WIN_W / 2 - map->w + j);
	SDL_SetRenderDrawColor(ctx->rend, 0, 0, 0, 255);
}

void tutorial_renderer(map_t *map, player_t *pl)
{
	maze_game_context_t *ctx = game_ctx();
	double posX = pl->x, posY = pl->y;  //x and y start position
	double dirX = cos(pl->view), dirY = sin(pl->view); //initial direction vector
	double planeX = 0, planeY = 0.66; //the 2d raycaster version of camera plane
	double w = WIN_W, h = WIN_H;

	// printf("dir = (%f, %f)\n", dirX, dirY);

	// double time = 0; //time of current frame
	// double oldTime = 0; //time of previous frame
	for(int x = 0; x < w; x++)
	{
			//calculate ray position and direction
			double cameraX = 2 * x / (double)w - 1; //x-coordinate in camera space
			double rayDirX = dirX + planeX * cameraX;
			double rayDirY = dirY + planeY * cameraX;
			//which box of the map we're in
			int mapX = (int)(posX);
			int mapY = (int)(posY);

			//length of ray from current position to next x or y-side
			double sideDistX;
			double sideDistY;

			double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
			double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);



			double perpWallDist;

			//what direction to step in x or y-direction (either +1 or -1)
			int stepX;
			int stepY;

			int hit = 0; //was there a wall hit?
			int side; //was a NS or a EW wall hit?
			//calculate step and initial sideDist
			if(rayDirX < 0)
			{
				stepX = -1;
				sideDistX = (posX - mapX) * deltaDistX;
			}
			else
			{
				stepX = 1;
				sideDistX = (mapX + 1.0 - posX) * deltaDistX;
			}
			if(rayDirY < 0)
			{
				stepY = -1;
				sideDistY = (posY - mapY) * deltaDistY;
			}
			else
			{
				stepY = 1;
				sideDistY = (mapY + 1.0 - posY) * deltaDistY;
			}
			//perform DDA
			while(hit == 0)
			{
				//jump to next map square, either in x-direction, or in y-direction
				if(sideDistX < sideDistY)
				{
					sideDistX += deltaDistX;
					mapX += stepX;
					side = 0;
				}
				else
				{
					sideDistY += deltaDistY;
					mapY += stepY;
					side = 1;
				}

				if (map_get(map, mapX, mapY) != FLOOR)
					hit = 1;
			}
			/*
			Calculate distance projected on camera direction. This is the shortest distance from the point where the wall is
			hit to the camera plane. Euclidean to center camera point would give fisheye effect!
			This can be computed as (mapX - posX + (1 - stepX) / 2) / rayDirX for side == 0, or same formula with Y
			for size == 1, but can be simplified to the code below thanks to how sideDist and deltaDist are computed:
			because they were left scaled to |rayDir|. sideDist is the entire length of the ray above after the multiple
			steps, but we subtract deltaDist once because one step more into the wall was taken above.
			*/
			if(side == 0) perpWallDist = (sideDistX - deltaDistX);
			else          perpWallDist = (sideDistY - deltaDistY);

			/* Calculate height of line to draw on screen */
			int lineHeight = (int)(h / perpWallDist);

			/* calculate lowest and highest pixel to fill in current stripe */
			int drawStart = -lineHeight / 2 + h / 2;
			int drawEnd = lineHeight / 2 + h / 2;

			if(drawStart < 0) drawStart = 0;
			if(drawEnd >= h) drawEnd = h - 1;

			/* give x and y sides different brightness */
			if (side == 1)
				SDL_SetRenderDrawColor(ctx->rend, 255, 255, 255, 255);
			else
				SDL_SetRenderDrawColor(ctx->rend, 127, 127, 127, 255);
			SDL_RenderDrawLine(ctx->rend, x, drawStart, x, drawEnd);
		}
	SDL_SetRenderDrawColor(ctx->rend, 0, 0, 0, 255);
}

void distance_vh2(map_t *map, double x, double y, double theta,
				  double *dist, double *hdist, double *vdist, bool *north_or_south)
{
	double ix = (int)(x), iy = (int)(y), w = map->w, h = map->h,
		   dx = cos(theta), dy = sin(theta),
		   hdx = (dx >= 0) ? 1 : -1, vdy = (dy >= 0) ? 1 : -1,
		   hd = (dx >= 0) ? 1 + (floor(x) - x) : (floor(x) - x),
		   vd = (dy >= 0) ? 1 + (floor(y) - y) : (floor(y) - y),
		   thd, tvd, tan_th = tan(theta), itan_th = 1 / tan_th;

	if (map_get(map, ix, iy) == WALL)
	{
		*vdist = 0, *hdist = 0;
		return;
	}
	ix = x + hd, iy = y + (hd * tan_th);

	while (0 <= ix && ix < w && 0 <= iy && iy < h && map_get(map, ix, iy) == FLOOR)
	{
		hd += hdx;
		ix = x + hd, iy = y + (hd * tan_th);
		if (0 > iy || iy > h - 1 || map_get(map, ix, iy) != FLOOR)
		{
			if (dx < 0 && 0 <= (iy - hdx * tan_th) && (iy - hdx * tan_th) < h &&
				0 <= ix && ix < w && map_get(map, ix, iy - hdx * tan_th) != FLOOR)
				hd -= hdx;
			break;
		}
	}

	iy = y + vd;
	ix = x + (vd * itan_th);
	while (0 <= ix && ix < w && 0 <= iy && iy < h && map_get(map, ix, iy) == FLOOR)
	{
		vd += vdy;
		iy = y + vd, ix = x + (vd * itan_th);
		if (0 > ix || ix > w - 1 || 0 > iy || iy > h - 1 || map_get(map, ix, iy) != FLOOR)
		{
			if (dy < 0 && 0 <= (ix - vdy * itan_th) && (ix - vdy * itan_th) < w &&
				0 <= iy && iy < h && map_get(map, ix - vdy * itan_th, iy) != FLOOR)
				vd -= vdy;
			break;
		}
	}
	*hdist = hd, *vdist = vd;
	thd = hd / dx, tvd = vd / dy;
	if (fabs(tvd) <= fabs(thd))
		*north_or_south = true, *dist = (tvd);
	else
		*north_or_south = false, *dist = (thd);
}

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

void raycaster_textured(map_t *map, player_t *pl)
{
	maze_game_context_t *ctx = game_ctx();
	double d, finalx, finaly, hd, vd, h, px_angle_dt = FOV_W / WIN_W, tan_fh2 = tan(FOV_H / 2) * 2;
	bool north_or_south = false;
	char textbuf[100];


	for (int i = -(WIN_W / 2); i < WIN_W / 2; i++) {
		double tan_th = tan(pl->view + i * px_angle_dt), itan_th = 1 / tan_th;

		distance_vh2(map, pl->x, pl->y, pl->view + i * px_angle_dt,
						&d, &hd, &vd, &north_or_south);
		h = d * tan_fh2;

		if (north_or_south)
			finalx = vd * itan_th + pl->x,
			draw_texture_line(i + WIN_W/2, WIN_H * (0.5 - WALL_H / (2 * h)),
							  i + WIN_W/2, WIN_H * (0.5 + WALL_H / (2 * h)),
							  (((finalx) - floor(finalx))), ctx->texs->p[0]);
		else
			finaly = hd * tan_th + pl->y,
			draw_texture_line(i + WIN_W/2, WIN_H * (0.5 - WALL_H / (2 * h)),
							  i + WIN_W/2, WIN_H * (0.5 + WALL_H / (2 * h)),
							  (((finaly) - floor(finaly))), ctx->texs->p[1]);
	}
	snprintf(textbuf, 100, "fps: %.2f", 1 / ctx->dt);
	render_text(textbuf, 0, 0);

	SDL_SetRenderDrawColor(ctx->rend, 0, 0, 0, 255);
}

void raycaster(map_t *map, player_t *pl)
{
	maze_game_context_t *ctx = game_ctx();
	double d, hd, vd, h, px_angle_dt = FOV_W / WIN_W, tan_fh2 = tan(FOV_H / 2) * 2;
	bool north_or_south = false;

	for (int i = -(WIN_W / 2); i < WIN_W / 2; i++) {
		distance_vh2(map, pl->x, pl->y, pl->view + i * px_angle_dt,
						&d, &hd, &vd, &north_or_south);
		h = d * tan_fh2;

		if (north_or_south)
			SDL_SetRenderDrawColor(ctx->rend, 255, 255, 255, 255);
		else
			SDL_SetRenderDrawColor(ctx->rend, 127, 127, 127, 255);
		SDL_RenderDrawLine(ctx->rend,
			i + WIN_W/2, WIN_H * (0.5 - WALL_H / (2 * h)),
			i + WIN_W/2, WIN_H * (0.5 + WALL_H / (2 * h)));
	}
	char textbuf[100];
	snprintf(textbuf, 100, "fps: %.2f", 1 / ctx->dt);
	render_text(textbuf, 0, 0);

	SDL_SetRenderDrawColor(ctx->rend, 0, 0, 0, 255);
}


void raycaster_2D_preview(map_t *map, player_t *pl)
{
	maze_game_context_t *ctx = game_ctx();
	double SDL_UNUSED d, hd, vd, lineh,
		px_angle_dt = FOV_W / WIN_W, tan_fh2 = tan(FOV_H / 2) * 2;
	char stat_display_buf[100];
	bool north_or_south;

	CLAMP_ANGLE(pl->view);
	for (int ray_n = -(WIN_W / 2); ray_n < WIN_W / 2; ray_n+=8) {
		double  x = pl->x, y = pl->y, theta = pl->view + ray_n * px_angle_dt,
				w = map->w, h = map->h, tan_th = tan(theta), itan_th = 1 / tan_th;

		distance_vh2(map, pl->x, pl->y, pl->view + ray_n * px_angle_dt,
					&d, &hd, &vd, &north_or_south);
#define THICKNESS 1
		for (int i = 0; i < map->h; i++)
			for (int j = 0; j < map->w; j++) {
				SDL_Rect r = {
					.x = WIN_H*j/w + THICKNESS,
					.y = WIN_H*i/h + THICKNESS,
					.w = WIN_H/w - 2*THICKNESS,
					.h = WIN_H/h - 2*THICKNESS,
				};
				if (map_get(map, j, i) != FLOOR)
					SDL_SetRenderDrawColor(ctx->rend, 255, 255, 255, 255);
				else
					SDL_SetRenderDrawColor(ctx->rend, 100, 100, 100, 255);
				SDL_RenderDrawRect(ctx->rend, &r);
			}
		if (ray_n == 0)
		{
			SDL_Rect r = {
				.w = WIN_H/w - 2*THICKNESS,
				.h = WIN_H/h - 2*THICKNESS
			};
			if (north_or_south)
				r.x = WIN_H*(vd * itan_th + x)/w + THICKNESS,
				r.y = WIN_H*(vd + y)/h + THICKNESS,
				SDL_SetRenderDrawColor(ctx->rend, 30, 255, 50, 255);
			else
				r.x = WIN_H*(hd + x)/w + THICKNESS,
				r.y = WIN_H*(hd * tan_th + y)/h + THICKNESS,
				SDL_SetRenderDrawColor(ctx->rend, 30, 50, 255, 255);
			SDL_RenderDrawRect(ctx->rend, &r);
		}

		SDL_Rect p = { .x = pl->x / w * WIN_H - 4, .y = pl->y / h * WIN_H - 4, .w = 8, .h =  8};

		SDL_SetRenderDrawColor(ctx->rend, 180, 80, 20, 255);
		SDL_RenderFillRect(ctx->rend, &p);

		if (ctx->hoff % 3 == 1 || (north_or_south && ctx->hoff % 3 == 0)) {
			if (ray_n != 0)
				SDL_SetRenderDrawColor(ctx->rend, 30, 50, 255, 255),
				SDL_RenderDrawLine(ctx->rend, pl->x / w * WIN_H, pl->y / h * WIN_H,
				((vd * itan_th) + x) / w * WIN_H, (vd + y) / h * WIN_H);
			else
				SDL_SetRenderDrawColor(ctx->rend, 255, 50, 30, 255),
				SDL_RenderDrawLine(ctx->rend, pl->x / w * WIN_H - 1, pl->y / h * WIN_H + 1,
				((vd * itan_th) + x) / w * WIN_H - 1, (vd + y) / h * WIN_H + 1),
				SDL_RenderDrawLine(ctx->rend, pl->x / w * WIN_H, pl->y / h * WIN_H,
				((vd * itan_th) + x) / w * WIN_H, (vd + y) / h * WIN_H),
				SDL_RenderDrawLine(ctx->rend, pl->x / w * WIN_H + 1, pl->y / h * WIN_H - 1,
				((vd * itan_th) + x) / w * WIN_H + 1, (vd + y) / h * WIN_H - 1);
		} else {
			if (ray_n != 0)
				SDL_SetRenderDrawColor(ctx->rend, 30, 255, 50, 255),
				SDL_RenderDrawLine(ctx->rend, pl->x / w * WIN_H, pl->y / h * WIN_H,
				(hd + x) / w * WIN_H, ((hd * tan_th) + y) / h * WIN_H);
			else
				SDL_SetRenderDrawColor(ctx->rend, 255, 50, 30, 255),
				SDL_RenderDrawLine(ctx->rend, pl->x / w * WIN_H - 1, pl->y / h * WIN_H + 1,
				(hd + x) / w * WIN_H - 1, ((hd * tan_th) + y) / h * WIN_H + 1),
				SDL_RenderDrawLine(ctx->rend, pl->x / w * WIN_H, pl->y / h * WIN_H,
				(hd + x) / w * WIN_H, ((hd * tan_th) + y) / h * WIN_H),
				SDL_RenderDrawLine(ctx->rend, pl->x / w * WIN_H + 1, pl->y / h * WIN_H - 1,
				(hd + x) / w * WIN_H + 1, ((hd * tan_th) + y) / h * WIN_H - 1);
		}
	}

	double SDL_UNUSED ix = (int)(pl->x), iy = (int)(pl->y), w = map->w, h = map->h,
		   dx = cos(pl->view), dy = sin(pl->view),
		   vdx = dx / dy, hdy = dy / dx,
		   hdx = (dx >= 0) ? 1 : -1, vdy = (dy >= 0) ? 1 : -1,
		   thd, tvd, hx, hy, vx, vy;

	sprintf(stat_display_buf, "view = %.3f", pl->view * 180/PI);
	render_text(stat_display_buf, WIN_H + 4, ctx->fz/2 * __COUNTER__);
	sprintf(stat_display_buf, "hdx = %.3f", hdx);
	render_text(stat_display_buf, WIN_H + 4, ctx->fz/2 * __COUNTER__);
	sprintf(stat_display_buf, "hdy = %.3f", hdy);
	render_text(stat_display_buf, WIN_H + 4, ctx->fz/2 * __COUNTER__);
	sprintf(stat_display_buf, "vdx = %.3f", vdx);
	render_text(stat_display_buf, WIN_H + 4, ctx->fz/2 * __COUNTER__);
	sprintf(stat_display_buf, "vdy = %.3f", vdy);
	render_text(stat_display_buf, WIN_H + 4, ctx->fz/2 * __COUNTER__);
	sprintf(stat_display_buf, "mode = %d", game_ctx()->hoff % 13);
	render_text(stat_display_buf, WIN_H + 4, ctx->fz/2 * __COUNTER__);

	SDL_SetRenderDrawColor(ctx->rend, 0, 0, 0, 255);
}


#endif
