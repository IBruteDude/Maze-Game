#include "raycaster.h"

dvec2 nearest_wall_down_left(map_t *map, double x, double y, double theta)
{
	dvec2 result = {{ 0 * x * map->h, 0 * y * theta }};

	return (result);
}
dvec2 nearest_wall_down_right(map_t *map, double x, double y, double theta)
{
	dvec2 result = {{ 0 * x * map->h, 0 * y * theta }};

	return (result);
}
dvec2 nearest_wall_up_right(map_t *map, double x, double y, double theta)
{
	int xp = x, yp = y;
	double cutoff, tan_th = tan(theta);
	dvec2 result;

	x -= xp, y -= yp;

	while (0 < xp && xp < map->w && 0 < yp && yp < map->h)
	{
		cutoff = atan2(1 - y, 1 - x);

		if (theta < cutoff)
		{
			xp++, x = 0, y += (1 - x) * tan_th;
			if (y >= 1)
				y--, yp++;
			if (yp >= map->h)
				return (result.x = xp + x, result.y = yp - 1 + y, result);
		}
		else
		{
			yp++, y = 0, x += (1 - y) / tan_th;
			if (x >= 1)
				x--, xp++;
			if (xp >= map->w)
				return (result.x = xp - 1 + x, result.y = yp + y, result);
		}
		if (map_get(map, xp, yp) != FLOOR)
			break;
	}
	return (result.x = xp + x, result.y = yp + y, result);
}
dvec2 nearest_wall_up_left(map_t *map, double x, double y, double theta)
{
	dvec2 result = {{ 0 * x * map->h, 0 * y * theta }};

	return (result);
}
dvec2 find_nearest_wall(map_t *map, double x, double y, double theta)
{
	CLAMP_ANGLE(theta);
	/* -PI <= theta < -PI/2 */
	if (theta < -PI / 2)
		return (nearest_wall_down_left(map, x, y, theta));
	/* -PI/2 <= theta < 0 */
	if (theta < 0)
		return (nearest_wall_down_right(map, x, y, theta));
	/* 0 <= theta < PI/2 */
	if (theta < PI / 2)
		return (nearest_wall_up_right(map, x, y, theta));
	/* PI/2 <= theta < PI */
	return (nearest_wall_up_left(map, x, y, theta));
}

	/*
	direction_t dir;
	double corner_angles[4] = {	atan2(-y, 1 - x), atan2(1 - y, 1 - x),
								atan2(1 - y, -x), atan2(-y, -x) };
	theta = fmod(theta, 2 * PI);
	dir = (corner_angles[0] <= theta && theta < corner_angles[1]) ? RIGHT :
		(corner_angles[1] <= theta && theta < corner_angles[2]) ? UP :
		(corner_angles[2] <= theta && theta < corner_angles[3]) ? LEFT : DOWN;
	*/
