/*
 * ray.cpp
 *
 *  Created on: 11 Jun 2014
 *      Author: rick
 */

#include "ray.h"

Ray::Ray(){
}

Ray::~Ray(){
}

bool Ray::intersect(Vec3Df triangle){
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

