#include "Vec3D.hpp"

#ifndef INTERSECTION_H
#define INTERSECTION_H

#define OUTOFRANGE 10e9;

class Intersection
{
public:
	Vec3Df point;
	Vec3Df angle;
	Vec3Df color;
	bool hit;
	float distance;

	Intersection();
	~Intersection();
	Intersection(bool h);
	Intersection(bool h, Vec3Df p, float d);
};

#endif