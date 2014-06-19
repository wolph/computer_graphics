#include "Intersection.hpp"


Intersection::Intersection()
{
}


Intersection::~Intersection()
{
}

Intersection::Intersection(bool h){
	hit = h;
	distance = OUTOFRANGE;
}

Intersection::Intersection(bool h, Vec3Df p, float d, const Triangle *t){
	hit = h;
	point = p;
	distance = d;
	triangle = t;
}