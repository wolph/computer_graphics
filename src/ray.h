/*
 * ray.h
 *
 *  Created on: 11 Jun 2014
 *      Author: rick
 */

#ifndef RAY_H_
#define RAY_H_

#include "mesh.h"

class Ray{
private:
    Vec3Df orig;
    Vec3Df dest;
    Vec3Df color = Vec3Df(0, 1, 0);

public:
    Ray();
    Ray(const Vec3Df orig, const Vec3Df dest);
    Ray(Vec3Df color, const Vec3Df orig, const Vec3Df dest);
    virtual ~Ray();
    bool intersect(Vec3Df triangle);

};

#endif /* RAY_H_ */
