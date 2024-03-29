#include "maze_map.h"

dvec2 nearest_wall_down_left(map_t *map, double x, double y, double theta);
dvec2 nearest_wall_down_right(map_t *map, double x, double y, double theta);
dvec2 nearest_wall_up_right(map_t *map, double x, double y, double theta);
dvec2 nearest_wall_up_left(map_t *map, double x, double y, double theta);

dvec2 find_nearest_wall(map_t *map, double x, double y, double theta);
