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
    Vertex vertices[3] = {Vec3Df(0), Vec3Df(0), Vec3Df(0)};
    Texture textures[3] = {Vec3Df(0), Vec3Df(0), Vec3Df(0)};
    Vec3Df normal = Vec3Df(0);
    
    Triangle();
    Triangle(const Triangle & triangle);
    Triangle(const Vertex & v0, const Vertex & v1, const Vertex & v2,
                const Texture & t0, const Texture & t1, const Texture & t2);
    Triangle(const Vertex & v0, const Vertex & v1, const Vertex & v2);
    inline virtual ~Triangle(){}
    Triangle & operator=(const Triangle & t2);
    inline void calculateNormal(){
        float length = normal.getLength();
        if(length > 0){
            normal /= length;
        }
    }
};

#endif /* TRIANGLE_H */
