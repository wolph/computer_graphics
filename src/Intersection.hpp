#include "Vec3D.hpp"
#include "Triangle.hpp"

#ifndef INTERSECTION_H
#define INTERSECTION_H

#define OUTOFRANGE 10e9

class Intersection
{
public:
	const Vec3Df point;
	const Vec3Df angle;
	const Vec3Df color;
	const bool hit = false;
	const float distance = OUTOFRANGE;
	const Triangle *triangle;

	inline Intersection(){};
	inline ~Intersection(){};
	inline Intersection(bool h): hit(h), distance(OUTOFRANGE){};
	inline Intersection(bool h, Vec3Df p, float d, const Triangle *t):
        point(p), hit(h), distance(d), triangle(t){};
};

#endif