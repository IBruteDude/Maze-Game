#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#ifndef PATH_MAX
	#ifdef __linux__
		#include "linux/limits.h"
	#elif defined(_WIN32)
		#define PATH_MAX	MAX_PATH
	#else
		#define PATH_MAX	1024
	#endif
#endif

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

#define PI 3.1415926535897932
#define CLAMP_ANGLE(theta) \
do {\
	if (theta < -PI) \
		theta = (-(fmod(-theta + PI, 2 * PI) - PI)); \
	else if (theta > PI)  \
		theta = fmod(theta + PI, 2 * PI) - PI; \
} while (0) \

#if !defined(MIN) && !defined(MAX) && !defined(ABS)
	#define MIN(x, y) ((((x) < (y)) ? (x) : (y)))
	#define MAX(x, y) ((((x) > (y)) ? (x) : (y)))
	#define ABS(x) ((((x) < 0) ? -(x) : (x)))
#endif

#ifdef DEBUG
	#define ASSERT(condition, x) \
		(assert(condition), x)

	#define DEBUGI(x) \
	(printf("L%d: " #x " = %ld\n", __LINE__, (long)x), fflush(stdout), x)
	#define DEBUG2I(x, y) \
	(printf("L%d: " #x " = %ld, " #y " = %ld\n", __LINE__, (long)x, (long)y), fflush(stdout), x), y

	#define DEBUGF(x) \
	(printf("L%d: " #x " = %f\n", __LINE__, (double)x), fflush(stdout), x)
	#define DEBUG2F(x, y) \
	(printf("L%d: " #x " = %f, " #y " = %f\n", __LINE__, (double)x, (double)y), fflush(stdout), x), y

	#define DEBUGMAP(map, ix, iy, s) \
	do { \
		for (int i = 0; i < h; i++) \
		{ \
			for (int j = 0; j < w; j++) \
				printf((i == floor(ix) && j == floor(iy)) ? s "%d" s : " %d ",\
					map[i * w + j]); \
			putchar('\n'); \
		} \
		fflush(stdout); \
	} while (0)
#else
	#define DEBUGI(x)		x
	#define DEBUG2I(x, y)	x, y
	#define DEBUGF(x)		x
	#define DEBUG2F(x, y)	x, y
	#define DEBUGMAP(map, ix, iy, s)
#endif

#define SIGN(x) ((x != 0) * (x >= 0 ? 1 : -1))

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#if !SDL_VERSION_ATLEAST(2, 0, 10)

	#define SDL_RenderCopyF			SDL_RenderCopy
	#define SDL_RenderCopyExF		SDL_RenderCopyEx
	#define SDL_RenderDrawLineF		SDL_RenderDrawLine
	#define SDL_RenderDrawLinesF	SDL_RenderDrawLines
	#define SDL_RenderDrawPointF	SDL_RenderDrawPoint
	#define SDL_RenderDrawPointsF	SDL_RenderDrawPoints
	#define SDL_RenderDrawRectF		SDL_RenderDrawRect
	#define SDL_RenderDrawRectsF	SDL_RenderDrawRects
	#define SDL_RenderFillRectF		SDL_RenderFillRect
	#define SDL_RenderFillRectsF	SDL_RenderFillRects

	#define SDL_FRect				SDL_Rect
	#define SDL_FPoint				SDL_Point

#endif

#define WIN_W 1024
#define WIN_H 768

#include <DG_dynarr.h>

DA_TYPEDEF(byte, byte_vec)

#include "cjson/cJSON.h"

#define JOBJ(obj, field) cJSON_GetObjectItem(obj, field)

#define JBOOL(obj) cJSON_IsTrue(obj)
#define JNUM(obj) cJSON_GetNumberValue(obj)
#define JSTR(obj) cJSON_GetStringValue(obj)
#define JARR_GET(obj, idx)	cJSON_GetArrayItem(obj, idx)
#define JARR_SIZE(obj)	cJSON_GetArraySize(obj)

#define json_bool(obj, field) JBOOL(JOBJ(obj, field))
#define json_num(obj, field) JNUM(JOBJ(obj, field))
#define json_str(obj, field) JSTR(JOBJ(obj, field))
#define json_arr_get(obj, field, idx)	JARR_GET(JOBJ(obj, field), idx)
#define json_arr_size(obj, field)	JARR_SIZE(JOBJ(obj, field))


#define ALL2(xs, pred) ((pred((xs)[0])) && (pred((xs)[1])))
#define ALL3(xs, pred) ((pred((xs)[0])) && (pred((xs)[1])) && (pred((xs)[2])))
#define ALL4(xs, pred) ((pred((xs)[0])) && (pred((xs)[1])) && (pred((xs)[2])) && (pred((xs)[3])))

#define NONE2(xs, pred) (!(pred((xs)[0])) && !(pred((xs)[1])))
#define NONE3(xs, pred) (!(pred((xs)[0])) && !(pred((xs)[1])) && !(pred((xs)[2])))
#define NONE4(xs, pred) (!(pred((xs)[0])) && !(pred((xs)[1])) && !(pred((xs)[2])) && !(pred((xs)[3])))

#define SOME2(xs, pred) ((pred((xs)[0])) || (pred((xs)[1])))
#define SOME3(xs, pred) ((pred((xs)[0])) || (pred((xs)[1])) || (pred((xs)[2])))
#define SOME4(xs, pred) ((pred((xs)[0])) || (pred((xs)[1])) || (pred((xs)[2])) || (pred((xs)[3])))

#define APPLY2(xs, func) \
do { \
	(func((xs)[0])); \
	(func((xs)[1])); \
} while (0)
#define APPLY3(xs, func) \
do { \
	(func((xs)[0])); \
	(func((xs)[1])); \
	(func((xs)[2])); \
} while (0)
#define APPLY4(xs, func) \
do { \
	(func((xs)[0])); \
	(func((xs)[1])); \
	(func((xs)[2])); \
	(func((xs)[3])); \
} while (0)

#define maze_loge(err_src) fprintf(stderr, \
	#err_src " Error: %s\n", SDL_GetError())

#define VEC4_T(type, name) \
typedef union name##4 \
{ \
	struct \
	{ \
		type x, y, z, w; \
	}; \
	struct \
	{ \
		type r, g, b, a; \
	}; \
} name##4; \

#define VEC3_T(type, name) \
typedef union name##3 \
{ \
	struct \
	{ \
		type x, y, z; \
	}; \
	struct \
	{ \
		type r, g, b; \
	}; \
} name##3; \

#define VEC2_T(type, name) \
typedef union name##2 \
{ \
	struct \
	{ \
		type x, y; \
	}; \
	struct \
	{ \
		type r, g; \
	}; \
} name##2; \

#define VEC_T(type, prefix) \
	VEC2_T(type, prefix) VEC3_T(type, prefix) VEC4_T(type, prefix)

VEC_T(bool, bvec)
VEC_T(int, ivec)
VEC_T(unsigned int, uvec)
VEC_T(float, vec)
VEC_T(double, dvec)

/**
 * struct player_s - a struct for storing player data
 *
 * @x: x coordinate of the player on the map
 * @y: y coordinate of the player on the map
 * @pos: a 2d vector storing x, y coordinates together
 * @view: the view angle of the player
 * @speed: the speed of movement of the player
 * @xvel: the direction of horizontal movement
 * @yvel: the direction of vertical movement
 */
typedef struct player_s
{
	union
	{
		struct
		{
			double x, y;
		};
		dvec2 pos;
	};
	double view;
	double speed;
	double xvel, yvel;
} player_t;

char *loadfile(const char *filename);

#endif
