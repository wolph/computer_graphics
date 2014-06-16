/*
 * Triangle.hpp
 *
 *  Created on: 12 Jun 2014
 *      Author: rick
 */

#ifndef TRIANGLE_H
#define TRIANGLE_H

/* fixed recursive includes */
class Triangle;

#include "Vec3D.hpp"
#include "Vertex.hpp"

class Triangle{
public:
    Vertex vertices[3];
    unsigned int textures[3];
    Vec3Df normal;
    Triangle();
    Triangle(const Triangle & triangle);
    Triangle(Vertex v0, unsigned int t0, Vertex v1, unsigned int t1, Vertex v2,
            unsigned int t2);
    virtual ~Triangle();
    Triangle & operator=(const Triangle & t2);
    void calculateNormal();
};

#endif /* TRIANGLE_H */
