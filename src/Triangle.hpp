/*
 * Triangle.hpp
 *
 *  Created on: 12 Jun 2014
 *      Author: rick
 */

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Vec3D.hpp"

class Triangle{
public:
    unsigned int v[3];
    unsigned int t[3];
	Vec3Df normal;
    Triangle();
    Triangle(const Triangle & t2);
    Triangle(unsigned int v0, unsigned int t0, unsigned int v1, unsigned int t1,
            unsigned int v2, unsigned int t2);
    virtual ~Triangle();
    Triangle & operator=(const Triangle & t2);
};

#endif /* TRIANGLE_H */
