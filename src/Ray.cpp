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

float Ray::intersect(Triangle& triangle){
    // The cosine of the angle of the vectors (dotproduct of the vectors).
    const float angle = dot(triangle.normal, dir);

    /* Floats are only rarely exactly 0, are you sure this is correct? */
    if(abs(angle) < 0.01){
        // If the ray and the plane are parallel (so their angle is 0), they won't intersect.
        return VEWY_HIGH;
    }
    Vec3Df& v0 = triangle.vertices[0].p;
    Vec3Df& v1 = triangle.vertices[1].p;
    Vec3Df& v2 = triangle.vertices[2].p;
    Vec3Df& normal = triangle.normal;

    // The distance of the ray's origin
    const float rayD = -(dot(triangle.normal, orig) + dot(triangle.normal, v0))
    / angle;
    // to the plane in which lies the triangle.

    const Vec3Df rayHit = orig + rayD * dir;// The intersection point of the ray and the plane in which lies the triangle.

    if(dot(normal, cross(v1 - v0, rayHit - v0)) < 0)
    return VEWY_HIGH;
    if(dot(normal, cross(v2 - v1, rayHit - v1)) < 0)
    return VEWY_HIGH;
    if(dot(normal, cross(v0 - v2, rayHit - v2)) < 0)
    return VEWY_HIGH;

    return rayD;
}
