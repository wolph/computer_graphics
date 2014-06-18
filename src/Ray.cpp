/*
 * Ray.cpp
 *
 *  Created on: 11 Jun 2014
 *      Author: rick
 */

#include "Ray.hpp"

Ray::~Ray(){
}

const Vec3Df& Ray::getColor() const{
    return color;
}

void Ray::setColor(const Vec3Df& color){
    this->color = color;
}

void Ray::setColor(const float red, const float green, const float blue){
    this->color[0] = red;
    this->color[1] = green;
    this->color[2] = blue;
}

float Ray::intersect(const Triangle* triangle){
    float det, inv_det, u, v, t;

    Vec3Df e1 = triangle->vertices[1].p - triangle->vertices[0].p;
    Vec3Df e2 = triangle->vertices[2].p - triangle->vertices[0].p;

    Vec3Df P = cross(dir, e2);
    det = dot(e1, P);

    if(abs(det) < 0.000000001)
        return VEWY_HIGH;

    inv_det = 1.0f / det;

    Vec3Df T = orig - triangle->vertices[0].p;

    u = dot(T, P) * inv_det;
    if(u < 0.0f || u > 1.0f)
        return VEWY_HIGH;

    Vec3Df Q = cross(T, e1);

    v = dot(dir, Q) *inv_det;
    if(v < 0.0f || u + v > 1.0f)
        return VEWY_HIGH;

    t = dot(e2, Q) * inv_det;

    if(t > 0.00000001){
        return t;
    }
    return VEWY_HIGH;
}
