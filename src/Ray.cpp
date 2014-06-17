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

const Vec3Df& Ray::getDest() const{
    return dest;
}

const Vec3Df& Ray::getDir() const{
    return dir;
}

const Vec3Df& Ray::getOrig() const{
    return orig;
}

bool Ray::intersect(Vec3Df& triangle){
//     T tmin = (min.x - r.orig.x) / r.dir.x;
//     T tmax = (max.x - r.orig.x) / r.dir.x;
//     if (tmin > tmax) swap(tmin, tmax);
//     T tymin = (min.y - r.orig.y) / r.dir.y;
//     T tymax = (max.y - r.orig.y) / r.dir.y;
//     if (tymin > tymax) swap(tymin, tymax);
//     if ((tmin > tymax) || (tymin > tmax))
//         return false;
//     if (tymin > tmin)
//         tmin = tymin;
//     if (tymax < tmax)
//         tmax = tymax;
//     T tzmin = (min.z - r.orig.z) / r.dir.z;
//     T tzmax = (max.z - r.orig.z) / r.dir.z;
//     if (tzmin > tzmax) swap(tzmin, tzmax);
//     if ((tmin > tzmax) || (tzmin > tmax))
//         return false;
//     if (tzmin > tmin)
//         tmin = tzmin;
//     if (tzmax < tmax)
//         tmax = tzmax;
//     if ((tmin > r.tmax) || (tmax < r.tmin)) return false;
//     if (r.tmin < tmin) r.tmin = tmin;
//     if (r.tmax > tmax) r.tmax = tmax;
    return true;
}

