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
#include "Material.hpp"

class Triangle {
public:
    Vertex vertices[3];
    Vec3Df textures[3];
    Vec3Df normal;
	const Material& material;
    Triangle(const Triangle & triangle);
    Triangle(const Vertex & v0, const Vertex & v1, const Vertex & v2,
                const Vec3Df & t0, const Vec3Df & t1, const Vec3Df & t2, const Material & m);
    Triangle(const Vertex & v0, const Vertex & v1, const Vertex & v2, const Material & m);
    inline ~Triangle(){};
    void calculateNormal();
};

#endif /* TRIANGLE_H */
