#ifndef MAX_H
#define MAX_H
#include "../Ray.hpp"

unsigned int raysTraced;

Ray performRayTracingMax(Ray ray);
bool intersect(Triangle t, Ray ray);

#endif
