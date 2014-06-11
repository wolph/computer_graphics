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
public:
    Ray();
    virtual ~Ray();
    bool intersect(Vec3Df triangle);
};

#endif /* RAY_H_ */
