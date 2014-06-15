#ifndef MAX_H
#define MAX_H
#include "../Ray.hpp"

Ray performRayTracingMax(Ray ray);
float intersect(Triangle t, Ray ray);
Vec3Df cross(Vec3Df in1, Vec3Df in2);
float dot(Vec3Df in1, Vec3Df in2);
Vec3Df normal(Vec3Df in);

#endif
