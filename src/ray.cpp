/*
 * Ray.cpp
 *
 *  Created on: 11 Jun 2014
 *      Author: rick
 */

#include "Ray.hpp"

Intersection FalseIntersection = Intersection(false);

const Intersection Ray::intersect(const Triangle* const triangle){
    const Vertex* vertices = triangle->vertices;
    const Vec3Df& v0 = vertices[0].p;
    const Vec3Df& v1 = vertices[1].p;
    const Vec3Df& v2 = vertices[2].p;

    const Vec3Df e1 = v1 - v0;
    const Vec3Df e2 = v2 - v0;

    const Vec3Df P = cross(dir, e2);
    const float det = dot(e1, P);

    if(abs(det) < 0.000000001)
        return FalseIntersection;

    const float inv_det = 1.0f / det;

    const Vec3Df T = orig - vertices[0].p;

    const float u = dot(T, P) * inv_det;
    if(u < 0.0f || u > 1.0f)
        return FalseIntersection;

    const Vec3Df Q = cross(T, e1);

    const float v = dot(dir, Q) *inv_det;
    if(v < 0.0f || u + v > 1.0f)
        return FalseIntersection;

    const float t = dot(e2, Q) * inv_det;

    if(t > 0.00000001){
        return Intersection(true, Vec3Df(0, 0, 0), t, triangle);
    }
    return FalseIntersection;
}