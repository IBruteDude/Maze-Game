#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

#define PI 3.1415926535897932
#define CLAMP_ANGLE(theta) \
do \
	if (theta < -PI) \
		theta = (-(fmod(-theta + PI, 2 * PI) - PI)); \
	else if (PI < theta)  \
		theta = fmod(theta + PI, 2 * PI) - PI; \
while (0) \

#if !defined(MIN) && !defined(MAX) && !defined(ABS)
	#define MIN(x, y) ((((x) < (y)) ? (x) : (y)))
	#define MAX(x, y) ((((x) > (y)) ? (x) : (y)))
	#define ABS(x) ((((x) < 0) ? -(x) : (x)))
#endif

#ifndef DBG
	#define DEBUGI(x) printf("L%d: " #x " = %ld\n", __LINE__, (long)x)
	#define DEBUG2I(x, y) printf("L%d: " #x " = %ld, " #y " = %ld\n", __LINE__, (long)x, (long)y)

	#define DEBUGF(x) printf("L%d: " #x " = %f\n", __LINE__, (double)x)
	#define DEBUG2F(x, y) printf("L%d: " #x " = %f, " #y " = %f\n", __LINE__, (double)x, (double)y)

	#define DEBUGMAP(map, ix, iy, s) \
	do { \
		for (int i = 0; i < h; i++) { \
			for (int j = 0; j < w; j++) \
				printf((i == floor(ix) && j == floor(iy)) ? s "%d" s : " %d ", map[i * w + j]); \
			putchar('\n'); \
		} \
	} while(0)
#else
	#define DEBUGI(x)
	#define DEBUG2I(x, y)
	#define DEBUGF(x)
	#define DEBUG2F(x, y)
	#define DEBUGMAP(map, ix, iy, s)
#endif

#define SIGN(x) ((x != 0) * (x >= 0 ? 1 : -1))

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define WIN_W 640
#define WIN_H 480

#include <DG_dynarr.h>

DA_TYPEDEF(byte, byte_vec)

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

typedef enum direction_e
{
	UP		= 0,
	DOWN	= 1,
	RIGHT	= 2,
	LEFT	= 3,
} direction_t;

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
} player_t;

#endif
