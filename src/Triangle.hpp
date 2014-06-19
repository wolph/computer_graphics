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
    const Vertex vertices[3];
    Texture textures[3];
    Vec3Df normal;
    inline Triangle(): vertices{Vertex(0), Vertex(0), Vertex(0)},
    textures{Texture(0), Texture(0), Texture(0)}, normal(0){};
    inline Triangle(const Triangle & triangle) :
        vertices{triangle.vertices[0], triangle.vertices[1], triangle.vertices[2]},
        textures{triangle.textures[0], triangle.textures[1], triangle.textures[2]},
        normal(triangle.normal)
        {};
    inline Triangle(const Vertex & v0, const Vertex & v1, const Vertex & v2,
            const Texture & t0, const Texture & t1, const Texture & t2):
        vertices{v0, v1, v2},
        textures{t0, t1, t2},
        normal(calculateNormal(v0.p, v1.p, v2.p))
        {};

    inline Triangle(const Vertex & v0, const Vertex & v1, const Vertex & v2):
    vertices{v0, v1, v2}
    {};
    inline virtual ~Triangle(){}
//    inline Triangle & operator=(const Triangle & t2);
    static inline Vec3Df calculateNormal(const Vec3Df& v0, const Vec3Df& v1, const Vec3Df& v2){
        Vec3Df normal = Vec3Df::crossProduct(v1 - v0, v2 - v1);
        float length = normal.getLength();
        if(length > 0)
            normal /= normal.getLength();
        return normal;
    }

    inline void calculateNormal(){
        normal = calculateNormal(vertices[0].p, vertices[1].p, vertices[2].p);
    }
};

#endif /* TRIANGLE_H */
