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
#include "Texture.hpp"

class Triangle{
public:
    Vertex vertices[3];
    Texture textures[3];
    Vec3Df normal;
    Triangle();
    Triangle(const Triangle & triangle);
    Triangle(const Vertex & v0, const Vertex & v1, const Vertex & v2,
            const Texture & t0, const Texture & t1, const Texture & t2);
    virtual ~Triangle();
    Triangle & operator=(const Triangle & t2);
    void calculateNormal();
};

#endif /* TRIANGLE_H */
