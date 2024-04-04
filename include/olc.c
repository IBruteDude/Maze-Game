#include "olc.h"

void olc_distance(map_t *map, double x, double y, double theta,
		double *dist, double *hdist, double *vdist, bool *north_or_south)
{
	// Form ray cast from player into scene
	vec2 vRayStart = { .x = x, .y = y };
	vec2 vRayDir = { .x = cos(theta), .y = sin(theta) };
			
	// Lodev.org also explains this additional optimistaion (but it's beyond scope of video)
	// vec2 vRayUnitStepSize = { abs(1.0f / vRayDir.x), abs(1.0f / vRayDir.y) };

	vec2 vRayUnitStepSize = { .x = sqrt(1 + (vRayDir.y / vRayDir.x) * (vRayDir.y / vRayDir.x)), .y = sqrt(1 + (vRayDir.x / vRayDir.y) * (vRayDir.x / vRayDir.y)) };
	ivec2 vMapCheck = { .x = vRayStart.x, .y = vRayStart.y };
	vec2 vRayLength1D;
	ivec2 vStep;

	// Establish Starting Conditions
	if (vRayDir.x < 0)
	{
		vStep.x = -1;
		vRayLength1D.x = (vRayStart.x - (float)(vMapCheck.x)) * vRayUnitStepSize.x;
	}
	else
	{
		vStep.x = 1;
		vRayLength1D.x = ((float)(vMapCheck.x + 1) - vRayStart.x) * vRayUnitStepSize.x;
	}

	if (vRayDir.y < 0)
	{
		vStep.y = -1;
		vRayLength1D.y = (vRayStart.y - (float)(vMapCheck.y)) * vRayUnitStepSize.y;
	}
	else
	{
		vStep.y = 1;
		vRayLength1D.y = ((float)(vMapCheck.y + 1) - vRayStart.y) * vRayUnitStepSize.y;
	}

	// Perform "Walk" until collision or range check
	bool bTileFound = false;
	float fMaxDistance = 100.0f;
	float fDistance = 0.0f;
	while (!bTileFound && fDistance < fMaxDistance)
	{
		// Walk along shortest path
		if (vRayLength1D.x < vRayLength1D.y)
		{
			vMapCheck.x += vStep.x;
			fDistance = vRayLength1D.x;
			vRayLength1D.x += vRayUnitStepSize.x;
			*north_or_south = false;
		}
		else
		{
			vMapCheck.y += vStep.y;
			fDistance = vRayLength1D.y;
			vRayLength1D.y += vRayUnitStepSize.y;
			*north_or_south = true;
		}

		// Test tile at new test point
		bTileFound = !(0 <= vMapCheck.x && vMapCheck.x < map->w && 0 <= vMapCheck.y && vMapCheck.y < map->h && map_get(map, vMapCheck.x, vMapCheck.y) == FLOOR);
	}

	// Calculate intersection location
	vec2 vIntersection = { .x = vRayStart.x + vRayDir.x * fDistance, .y = vRayStart.y + vRayDir.y * fDistance };

/*****************************************************************************/
	*vdist = vRayDir.y * fDistance;
	*hdist = vRayDir.x * fDistance;
	*dist = sqrt((vIntersection.x - x) * (vIntersection.x - x) + (vIntersection.y - y) * (vIntersection.y - y));
}

