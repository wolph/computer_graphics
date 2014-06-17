#ifndef MAX_H
#define MAX_H
#include "../Ray.hpp"

void performRayTracingMax(Ray& ray);
float intersect(const Triangle& t, const Ray& ray);
inline Vec3Df cross(const Vec3Df& in1, const Vec3Df& in2);
inline float dot(const Vec3Df& in1, const Vec3Df& in2);
inline const Vec3Df normal(const Vec3Df& in);

#endif
