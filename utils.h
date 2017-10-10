#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <iostream>
#include "GameObjects.h"

namespace Collision {
	bool SphereCollision(Sphere a, Sphere b);
	bool SpherePlaneCollision(Sphere s, float a, float b, float c, float d);
	bool PointLineCollision(float coordinates[2][2], float x, float y);
}


#endif
