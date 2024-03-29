#ifndef MAZE_RENDER_H
#define MAZE_RENDER_H

#include "maze_base.h"
#include "raycaster.h"

#define FOV_W (60 * PI / 180)
#define FOV_H (FOV_W * WIN_H / WIN_W)

#define WALL_H 1.f

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


void render_fov(map_t *map, player_t *pl)
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
	// map_blk_t up, down, right, left;
	// assert(x - 1 >= 0 && x + 1 <= map->w - 1);
	// assert(y - 1 >= 0 && y + 1 <= map->h);
	// up = map_get(map, x - 1, y), down = map_get(map, x + 1, y);
	// right = map_get(map, x, y + 1), left = map_get(map, x, y - 1);
}

void all_vector_raycast_renderer(map_t *map, player_t *pl)
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
			// int lineHeight = (int)(h / perpWallDist);
			const double tan_fh2 = tan(FOV_H / 2) * 2;
			double lineHeight = perpWallDist * tan_fh2;

			/* calculate lowest and highest pixel to fill in current stripe */
			// int drawStart = -lineHeight / 2 + h / 2;
			// int drawEnd = lineHeight / 2 + h / 2;
			int drawStart = -WIN_H / (lineHeight * 2) + WIN_H / 2;
			int drawEnd =  WIN_H / (lineHeight * 2) + WIN_H / 2;

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


#define PREC 100
double distance_approximate(map_t *map, double x, double y, double theta, bool *side)
{
	// CLAMP_ANGLE(theta);

	int ix = floor(x), iy = floor(y);
	double  w = map->w, h = map->h,
			copyx = x, copyy = y,
			dx = cos(theta) / 10, dy = sin(theta) / 10;

	if (ix >= w || ix < 0 || iy >= map->h || iy < 0 || map_get(map, ix, iy) == 1)
		return (0);

	do {
		x += dx;
		y += dy;

		ix = floor(x), iy = floor(y);
		if (ix >= w || ix < 0 || iy >= h || iy < 0)
			break;
 	} while (map_get(map, ix, iy) == 1);
	if (x == ix)
		*side = false;
	else if (y == iy)
		*side = true;
	else if (dx != 0)
	{
		if (dy / dx > (y - iy) / (x - ix))
			y = iy, *side = true;
		else
			x = ix, *side = false;
	}
	// x -= dx, y -= dy;
	return (sqrt((x - copyx) * (x - copyx) + (y - copyy) * (y - copyy)));
}


double distance_vh(map_t *map, double x, double y, double theta, bool *vertical_side)
{
	// maze_game_context_t *ctx = game_ctx();

	double  ix = (int)(x), iy = (int)(y), w = map->w, h = map->h,
			dx = cos(theta), dy = sin(theta),
			// fx = x - ix, fy = y - iy,
			// hd = (dx >= 0) ? 1 - fx : -fx, vd = (dy >= 0) ? 1 - fy : -fy,
			// hd = (dx >= 0) ? 1 - fx : -fx, vd = (dy >= 0) ? 1 - fy : -fy,
			vdx = dx / dy, hdy = dy / dx,
			hdx = (dx >= 0) ? 1 : -1, vdy = (dy >= 0) ? 1 : -1,
			thd, tvd, hx, hy, vx, vy;

	assert(x >= 0 && y >= 0);
	if (map_get(map, ix, iy) == WALL)
		return (0);

	do {
		ix += hdx;
		iy += hdy;
	} while (0 <= ix && ix < w && 0 <= iy && iy < h && map_get(map, ix, iy) == FLOOR);

	hx = ix - hdx - x, hy = iy - hdy - y;
	thd = sqrt(hx * hx + hy * hy);

	ix = (int)(x);
	iy = (int)(y);
	do {
		ix += vdx;
		iy += vdy;
	} while (0 <= ix && ix < w && 0 <= iy && iy < h && map_get(map, ix, iy) == FLOOR);

	vx = ix - vdx - x, vy = iy - vdy - y;

	tvd = sqrt(vx * vx + vy * vy);
	if (tvd < thd) {
		*vertical_side = true;
		return (tvd);
	}
	*vertical_side = false;
	return (thd);
}

double distance_vh2(map_t *map, double x, double y, double theta, bool *north_or_south)
{

	double SDL_UNUSED ix = (int)(x), iy = (int)(y), w = map->w, h = map->h,
		   dx = cos(theta), dy = sin(theta),
		   vdx = dx / dy, hdy = dy / dx,
		   hdx = (dx >= 0) ? 1 : -1, vdy = (dy >= 0) ? 1 : -1,
		   hd = (dx >= 0) ? 1 + (int)(x) - x : ((int)(x) - x),
		   vd = (dy >= 0) ? 1 + (int)(y) - y : ((int)(y) - y),
		   thd, tvd, hx, hy, vx, vy, tan_th = tan(theta), itan_th = 1 / tan_th;

	assert(x >= 0 && y >= 0);
	if (map_get(map, ix, iy) == WALL)
		return (0);

	ix = x + hd;
	iy = y + (hd * tan_th);

	while (0 <= ix && ix < w && 0 <= iy && iy < h && map_get(map, ix, iy) == FLOOR)
	{
		hd += hdx;
		ix = x + hd;
		iy = y + (hd * tan_th);
	}

	// switch (game_ctx()->hoff % 13)
	// {
	// case 0:  hd -= hdx; break;
	// case 1:  hd -= hdx * (dy < 0); break;
	// case 2:  hd -= hdx * (dy >= 0); break;
	// case 3:  hd -= hdx * (dx < 0); break;
	// case 4:  hd -= hdx * (dx >= 0); break;
	// case 5:  hd -= hdx * (dx <  0 && dy <  0); break;
	// case 6:  hd -= hdx * (dx <  0 || dy <  0); break;
	// case 7:  hd -= hdx * (dx <  0 && dy >= 0); break;
	// case 8:  hd -= hdx * (dx <  0 || dy >= 0); break;
	// case 9:  hd -= hdx * (dx >= 0 && dy <  0); break;
	// case 10: hd -= hdx * (dx >= 0 || dy <  0); break;
	// case 11: hd -= hdx * (dx >= 0 && dy >= 0); break;
	// case 12: hd -= hdx * (dx >= 0 || dy >= 0); break;
	// case 13: hd -= hdx * (dx <  0 && dy <  0); break;
	// case 14: hd -= hdx * (dx <  0 || dy <  0); break;
	// case 15: hd -= hdx * (dx <  0 && dy >= 0); break;
	// case 16: hd -= hdx * (dx <  0 || dy >= 0); break;
	// case 17: hd -= hdx * (dx >= 0 && dy <  0); break;
	// case 18: hd -= hdx * (dx >= 0 || dy <  0); break;
	// case 19: hd -= hdx * (dx >= 0 && dy >= 0); break;
	// case 20: hd -= hdx * (dx >= 0 || dy >= 0); break;
	// }
	// hd -= (dx < 0 && !(hdx < 0 && (iy < 0 && iy > h - 1))) * hdx;

	iy = y + vd;
	ix = x + (vd * itan_th);
	while (0 <= ix && ix < w && 0 <= iy && iy < h && map_get(map, ix, iy) == FLOOR)
	{
		vd += vdy;
		iy = y + vd;
		ix = x + (vd * itan_th);
	}
	// switch (game_ctx()->hoff % 13)
	// {
	// case 0:  vd -= vdy; break;
	// case 1:  vd -= vdy * (dx < 0); break;
	// case 2:  vd -= vdy * (dx >= 0); break;
	// case 3:  vd -= vdy * (dy < 0); break;
	// case 4:  vd -= vdy * (dy >= 0); break;
	// case 5:  vd -= vdy * (dy < 0 &&  dx < 0); break;
	// case 6:  vd -= vdy * (dy < 0 ||  dx < 0); break;
	// case 7:  vd -= vdy * (dy < 0 &&  dx >= 0); break;
	// case 8:  vd -= vdy * (dy < 0 ||  dx >= 0); break;
	// case 9:  vd -= vdy * (dy >= 0 && dx < 0); break;
	// case 10: vd -= vdy * (dy >= 0 || dx < 0); break;
	// case 11: vd -= vdy * (dy >= 0 && dx >= 0); break;
	// case 12: vd -= vdy * (dy >= 0 || dx >= 0); break;
	// }
	// vd -= (dy < 0 && !(vdy < 0 && (ix < 0 && ix > w - 1))) * vdy;

	// thd = hd * sqrt(1 + (tan_th * tan_th));
	thd = hd / dx;
	// tvd = vd * sqrt(1 + (itan_th * itan_th));
	tvd = vd / dy;
	if (fabs(tvd) <= fabs(thd))
	{
		*north_or_south = true;
		return (tvd);
	}
	*north_or_south = false;
	return (thd);
}

void my_all_vector_raycast_renderer(map_t *map, player_t *pl)
{
	maze_game_context_t *ctx = game_ctx();
	double d, h,
		px_angle_dt = FOV_W / WIN_W, tan_fh2 = tan(FOV_H / 2) * 2;
	bool north_or_south = false;



	for (int i = -(WIN_W / 2); i < WIN_W / 2; i++) {
		d = distance_vh2(map, pl->x, pl->y, pl->view + i * px_angle_dt, &north_or_south);
		h = d * tan_fh2;

		if (north_or_south)
			SDL_SetRenderDrawColor(ctx->rend, 255, 255, 255, 255);
		else
			SDL_SetRenderDrawColor(ctx->rend, 127, 127, 127, 255);
		SDL_RenderDrawLine(ctx->rend,
			i + WIN_W/2, WIN_H * (0.5 - WALL_H / (2 * h)),
			i + WIN_W/2, WIN_H * (0.5 + WALL_H / (2 * h)));
		// DEBUGF(WIN_H * (0.5 - WALL_H / (2 * h)));
	}
	// SDL_SetRenderDrawColor(ctx->rend, 255, 127, 0, 255);

	// for (int i = 0; i < map->h; i++)
	// 	for (int j = 0; j < map->w; j++)
	// 		if (map_get(map, i, j) == WALL)
	// 			SDL_RenderDrawPoint(ctx->rend, WIN_H / 2 - map->h + i, WIN_W / 2 - map->w + j);
	SDL_SetRenderDrawColor(ctx->rend, 0, 0, 0, 255);
}

void render_text(const char *message, int x, int y) {
	maze_game_context_t *ctx = game_ctx();
	//this opens a font style and sets a size

	// this is the color in rgb format,
	// maxing out all would give you the color white,
	// and it will be your text's color
	SDL_Color White = { 255, 255, 255, 255 };

	// as TTF_RenderText_Solid could only be used on
	// SDL_Surface then you have to create the surface first
	SDL_Surface* surfaceMessage =
	    TTF_RenderText_Solid(ctx->font, message, White); 

	// now you can convert it into a texture
	SDL_Texture* Message = SDL_CreateTextureFromSurface(ctx->rend, surfaceMessage);

	SDL_Rect Message_rect; //create a rect
	Message_rect.x = x;  //controls the rect's x coordinate 
	Message_rect.y = y; // controls the rect's y coordinte
	Message_rect.h = ctx->fz / 2; // controls the height of the rect
	Message_rect.w = ctx->fz / 2 * strlen(message) / 3; // controls the width of the rect

	// (0,0) is on the top left of the window/screen,
	// think a rect as the text's box,
	// that way it would be very simple to understand

	// Now since it's a texture, you have to put RenderCopy
	// in your game loop area, the area where the whole code executes

	// you put the renderer's name first, the Message,
	// the crop size (you can ignore this if you don't want
	// to dabble with cropping), and the rect which is the size
	// and coordinate of your texture
	SDL_RenderCopy(ctx->rend, Message, NULL, &Message_rect);

	// Don't forget to free your surface and texture
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}


void raycasting_2D(map_t *map, player_t *pl)
{
	maze_game_context_t *ctx = game_ctx();
	double SDL_UNUSED d, lineh,
		px_angle_dt = FOV_W / WIN_W, tan_fh2 = tan(FOV_H / 2) * 2;
	bool north_or_south;
	char stat_display_buf[100];

	CLAMP_ANGLE(pl->view);
	for (int i = -(WIN_W / 2); i < WIN_W / 2; i++) {
		d = distance_vh(map, pl->x, pl->y, pl->view + i * px_angle_dt, &north_or_south);
	
		double x = pl->x, y = pl->y, theta = pl->view + i * px_angle_dt;

		double SDL_UNUSED ix = (int)(x), iy = (int)(y), w = map->w, h = map->h,
			   dx = cos(theta), dy = sin(theta),
			   vdx = dx / dy, hdy = dy / dx,
			   hdx = (dx >= 0) ? 1 : -1, vdy = (dy >= 0) ? 1 : -1,
			   hd = (dx >= 0) ? 1 + floor(x) - x : (floor(x) - x),
			   vd = (dy >= 0) ? 1 + floor(y) - y : (floor(y) - y),
			   thd, tvd, hx, hy, vx, vy, tan_th = tan(theta), itan_th = 1 / tan_th;

		assert(x >= 0 && y >= 0);
		if (map_get(map, ix, iy) == WALL)
			d = (0);
		else
		/*
		do
		{
			ix += hdx;
			iy += hdy;
		} while (0 <= ix && ix < w && 0 <= iy && iy < h && map_get(map, ix, iy) == FLOOR);

		hx = ix - hdx - x, hy = iy - hdy - y;
		thd = sqrt(hx * hx + hy * hy);

		ix = (int)(x);
		iy = (int)(y);
		do
		{
			ix += vdx;
			iy += vdy;
		} while (0 <= ix && ix < w && 0 <= iy && iy < h && map_get(map, ix, iy) == FLOOR);

		vx = ix - vdx - x, vy = iy - vdy - y;

		tvd = sqrt(vx * vx + vy * vy);
		if (tvd < thd)
			north_or_south = true, d = (tvd);
		else
			north_or_south = false, d = (thd);
		*/

{
	ix = x;// + hd;
	iy = y;// + (hd * tan_th);

	while (0 <= ix && ix < w && 0 <= iy && iy < h && map_get(map, ix, iy) == FLOOR)
	{
		hd += hdx;
		ix = x + hd;
		iy = y + (hd * tan_th);

		if (0 > iy || iy >= h || map_get(map, ix, iy) != FLOOR) {
			if (dx < 0 && 0 <= ix && ix < w && map_get(map, ix, iy - hdx * tan_th) != FLOOR)
				hd -= hdx;
			break;
		}
	}
	hd -= (dx < 0) * hdx;
	
	iy = y;// + vd;
	ix = x;// + (vd * itan_th);
	while (0 <= ix && ix < w && 0 <= iy && iy < h && map_get(map, ix, iy) == FLOOR)
	{
		vd += vdy;
		iy = y + vd;
		ix = x + (vd * itan_th);
		if (0 > ix || ix >= w || map_get(map, ix, iy) != FLOOR) {
			if (dy < 0 && 0 <= iy && iy < h && 0 <= (ix - vdy * itan_th) && (ix - vdy * itan_th) < w && map_get(map, ix - vdy * itan_th, iy) != FLOOR)
				vd -= vdy;	
			break;
		}
	}
	vd -= (dy < 0) * vdy;	

	// thd = hd * sqrt(1 + (tan_th * tan_th));
	thd = hd / dx;
	// tvd = vd * sqrt(1 + (itan_th * itan_th));
	tvd = vd / dy;
	if (fabs(tvd) <= fabs(thd))
	{
		north_or_south = true;
		d = (tvd);
	} else {
		north_or_south = false;
		d = (thd);
	}
}		// lineh = d * tan_fh2;

#define THICKNESS 1
		for (int i = 0; i < map->h; i++)
			for (int j = 0; j < map->w; j++) {
				SDL_Rect r = {
					.x = WIN_H*j/w + THICKNESS,
					.y = WIN_H*i/h + THICKNESS,
					.w = WIN_H/w - 2*THICKNESS,
					.h = WIN_H/h - 2*THICKNESS,
				};
				if (map_get(map, i, j) == WALL)
					SDL_SetRenderDrawColor(ctx->rend, 255, 255, 255, 255);
				else
					SDL_SetRenderDrawColor(ctx->rend, 100, 100, 100, 255);
				
				SDL_RenderDrawRect(ctx->rend, &r);
			}

		SDL_Rect p = { .x = pl->x / w * WIN_H - 4, .y = pl->y / h * WIN_H - 4, .w = 8, .h =  8};
		
		SDL_SetRenderDrawColor(ctx->rend, 180, 80, 20, 255);
		SDL_RenderFillRect(ctx->rend, &p);

		if (ctx->hoff % 3 == 0 || (fabs(tvd) <= fabs(thd) && ctx->hoff %3 == 1)) {
			if (i != 0)
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
			if (i != 0)
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

	// 	if (north_or_south)
	// 		SDL_SetRenderDrawColor(ctx->rend, 255, 255, 255, 255);
	// 	else
	// 		SDL_SetRenderDrawColor(ctx->rend, 127, 127, 127, 255);
	// 	SDL_RenderDrawLine(ctx->rend,
	// 		i + WIN_W/2, WIN_H * (0.5 - WALL_H / (2 * lineh)),
	// 		i + WIN_W/2, WIN_H * (0.5 + WALL_H / (2 * lineh)));
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
#undef PREC
#define PREC 10
#define INCR PI/180

void js_raycast_renderer(map_t *map, player_t *pl)
{
	CLAMP_ANGLE(pl->view);
	maze_game_context_t *ctx = game_ctx();
// 	int ix = floor(pl->x), iy = floor(pl->y);
// 	double  x = pl->x, y = pl->y,
// 			w = map->w, h = map->h,
// 			copyx = x, copyy = y,
// 			dx = cos(pl->view) / PREC, dy = sin(pl->view) / PREC;

// 	if (ix >= w || ix < 0 || iy >= map->h || iy < 0 || map_get(map, ix, iy) == 1)
// 		return (0);

// 	do {
// 		x += dx;
// 		y += dy;

// 		ix = floor(x), iy = floor(y);
// 		if (ix >= w || ix < 0 || iy >= h || iy < 0)
// 			break;
//  	} while (map_get(map, ix, iy) == 1);

// 	if (x != ix && dx != 0)
// 	{
// 		if (dy / dx > (y - iy) / (x - ix))
// 			y = iy;
// 		else
// 			x = ix;
// 	}
// 	x -= dx, y -= dy;
// 	return (sqrt((x - copyx) * (x - copyx) + (y - copyy) * (y - copyy)));

    double rayAngle = pl->view - FOV_W/2;
    for(int rayCount = 0; rayCount < WIN_W; rayCount++) {

        // Ray data
        player_t ray = {
            .x = pl->x,
            .y = pl->y
        };

        // Ray path incrementers
        double rayCos = cos(rayAngle * PI/180) / PREC;
        double raySin = sin(rayAngle * PI/180) / PREC;

        // Wall finder
        double wall = 0;
        while(wall == 0) {
            ray.x += rayCos;
            ray.y += raySin;
            wall = map_get(map, floor(ray.y), floor(ray.x));
        }

        // Pythagoras theorem
        double distance = sqrt(pow(pl->x - ray.x, 2) + pow(pl->y - ray.y, 2));

        // Fish eye fix
        distance = distance * cos(PI/180 * (rayAngle - pl->view));

        // Wall height
        double wallHeight = floor(WIN_H/2 / distance);

        // Draw
        // drawLine(rayCount, 0, rayCount, WIN_H/2 - wallHeight, "cyan");
        // drawLine(rayCount, WIN_H/2 - wallHeight, rayCount, WIN_H/2 + wallHeight, "red");
        // drawLine(rayCount, WIN_H/2 + wallHeight, rayCount, WIN_H, "green");

		SDL_SetRenderDrawColor(ctx->rend, 0, 0, 255, 255);
		SDL_RenderDrawLine(ctx->rend,
			rayCount + WIN_W/2, 0, rayCount + WIN_W/2, WIN_H/2 - wallHeight);
		SDL_SetRenderDrawColor(ctx->rend, 255, 0, 0, 255);
		SDL_RenderDrawLine(ctx->rend,
			rayCount + WIN_W/2, WIN_H/2 - wallHeight, rayCount + WIN_W/2, WIN_H/2 + wallHeight);
		SDL_SetRenderDrawColor(ctx->rend, 0, 255, 0, 255);
		SDL_RenderDrawLine(ctx->rend,
			rayCount + WIN_W/2, WIN_H/2 - wallHeight, rayCount + WIN_W/2, WIN_H);

        // Increment
        rayAngle += INCR;
    }
}


#define degToRad(x) ((x)*PI/180)

// void yoinked(map_t *map, player_t *pl)
// {
// 	double px = pl->x, py = pl->y, pa = pl->view, mapX = map->w, mapY = map->h, mapS = 1;
//  int r,mx,my,mp,dof; float vx,vy,rx,ry,ra,xo,yo,disV,disH; 
 
//  ra=(pa+30);                                                              //ray set back 30 degrees
 
// CLAMP_ANGLE(ra);

//  for(r=0;r<60;r++)
//  {
//   //---Vertical--- 
//   dof=0; disV=100000;
//   float Tan=tan(degToRad(ra));
//        if(cos(degToRad(ra))> 0.001){ rx=(((int)px>>6)<<6)+64;      ry=(px-rx)*Tan+py; xo= 64; yo=-xo*Tan;}//looking left
//   else if(cos(degToRad(ra))<-0.001){ rx=(((int)px>>6)<<6) -0.0001; ry=(px-rx)*Tan+py; xo=-64; yo=-xo*Tan;}//looking right
//   else { rx=px; ry=py; dof=8;}                                                  //looking up or down. no hit  

//   while(dof<8) 
//   { 
//    mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;                     
//    if(mp>0 && mp<mapX*mapY && map_get(map, mx, my)==1){ dof=8; disV=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);}//hit
//    else{ rx+=xo; ry+=yo; dof+=1;}                                               //check next horizontal
//   } 
//   vx=rx; vy=ry;

//   //---Horizontal---
//   dof=0; disH=100000;
//   Tan=1.0/Tan; 
//        if(sin(degToRad(ra))> 0.001){ ry=(((int)py>>6)<<6) -0.0001; rx=(py-ry)*Tan+px; yo=-64; xo=-yo*Tan;}//looking up 
//   else if(sin(degToRad(ra))<-0.001){ ry=(((int)py>>6)<<6)+64;      rx=(py-ry)*Tan+px; yo= 64; xo=-yo*Tan;}//looking down
//   else{ rx=px; ry=py; dof=8;}                                                   //looking straight left or right
 
//   while(dof<8) 
//   { 
//    mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;                          
//    if(mp>0 && mp<mapX*mapY && map_get(map, mx, my)==1){ dof=8; disH=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);}//hit         
//    else{ rx+=xo; ry+=yo; dof+=1;}                                               //check next horizontal
//   } 
  

//   if(disV<disH){ rx=vx; ry=vy; disH=disV; }                  //horizontal hit first

//   int ca=(pa-ra); CLAMP_ANGLE(ca); disH=disH*cos(degToRad(ca));                            //fix fisheye 
//   int lineH = (mapS*320)/(disH); if(lineH>320){ lineH=320;}                     //line height and limit
//   int lineOff = 160 - (lineH>>1);                                               //line offset
  
// //   glLineWidth(8);glBegin(GL_LINES);glVertex2i(r*8+530,lineOff);glVertex2i(r*8+530,lineOff+lineH);glEnd();//draw vertical wall  
// 	maze_game_context_t *ctx = game_ctx();

// 		SDL_SetRenderDrawColor(ctx->rend, 255, 255, 255, 255);
// 		SDL_RenderDrawLine(ctx->rend,
// 			r*8+WIN_W/2, lineOff,
// 			r*8+WIN_W/2, lineOff + lineH);
// 		SDL_SetRenderDrawColor(ctx->rend, 0, 0, 0, 255);

//   ra=(ra-1);                                                              //go to next ray
//   CLAMP_ANGLE(ra);
//  }
// }//-----------------------------------------------------------------------------



#endif
