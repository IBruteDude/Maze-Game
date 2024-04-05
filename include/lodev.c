#include "lodev.h"

void lodev_renderer(map_t *map, player_t *pl)
{
	maze_game_context_t *ctx = game_ctx();
	double posX = pl->x, posY = pl->y;  //x and y start position
	double dirX = cos(pl->view), dirY = sin(pl->view); //initial direction vector
	double planeX = 0, planeY = 0.66; //the 2d raycaster version of camera plane
	double w = WIN_W, h = WIN_H;

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
