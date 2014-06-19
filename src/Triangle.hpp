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
#include <array>

class Triangle{
public:
	const std::array<Vertex, 3> vertices;
	const std::array<Texture, 3> textures;
	Vec3Df normal;
	inline Triangle() : vertices({ Vertex(0), Vertex(0), Vertex(0) }) {}
   // textures{Texture(0), Texture(0), Texture(0)}, normal(0){};
    inline Triangle(const Triangle & triangle) :
        vertices({triangle.vertices[0], triangle.vertices[1], triangle.vertices[2]}),
        textures({triangle.textures[0], triangle.textures[1], triangle.textures[2]}),
        normal(triangle.normal)
        {};
    inline Triangle(const Vertex & v0, const Vertex & v1, const Vertex & v2,
            const Texture & t0, const Texture & t1, const Texture & t2):
        vertices({v0, v1, v2}),
        textures({t0, t1, t2}),
        normal(calculateNormal(v0, v1, v2))
        {};
    inline Triangle(const Vertex & v0, const Vertex & v1, const Vertex & v2,
		const Texture & t0, const Texture & t1, const Texture & t2, const Vec3Df n) :
		vertices({ v0, v1, v2 }),
		textures({ t0, t1, t2 }),
        normal(n)
        {};
	inline Triangle(const Vertex & v0, const Vertex & v1, const Vertex & v2) :
		vertices({ v0, v1, v2 })
    {};
    inline virtual ~Triangle(){}
//    inline Triangle & operator=(const Triangle & t){
//        return Triangle(
//            t.vertices[0],
//            t.vertices[1],
//            t.vertices[2],
//            t.textures[0],
//            t.textures[1],
//            t.textures[2],
//            t.normal
//        );
//    }
    static inline const Vec3Df calculateNormal(const Vertex& v0, const Vertex& v1, const Vertex& v2){
        Vec3Df normal = Vec3Df::crossProduct(v1.p - v0.p, v2.p - v1.p);
        float length = normal.getLength();
        if(length > 0)
            normal /= normal.getLength();

        return normal;
    }
};

#endif /* TRIANGLE_H */
