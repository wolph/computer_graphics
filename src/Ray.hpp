/*
 * Ray.hpp
 *
 *  Created on: 11 Jun 2014
 *      Author: rick
 */

#ifndef RAY_H
#define RAY_H

/* fixed recursive includes */
class Ray;

#include "Vec3D.hpp"
#include "Mesh.hpp"
#include "Intersection.hpp"

inline const Vec3Df normal(const Vec3Df& in){
    Vec3Df res = Vec3Df(in);
    res.normalize();
    return res;
}

class Ray{
public:
    Vec3Df color = Vec3Df(0, 0, 0);
    const Vec3Df orig;
    const Vec3Df dest;
    const Vec3Df dir;
    const unsigned int bounceCount = 10;

    inline Ray(Vec3Df& color_, const Vec3Df& orig_, const Vec3Df& dest_) :
            color(color_), orig(orig_), dest(dest_), dir(normal(dest_ - orig)){
    }
    inline Ray(Vec3Df& color_, const Vec3Df& orig_, const Vec3Df& dest_,
            const unsigned int bounceCount_) :
            color(color_), orig(orig_), dest(dest_), dir(normal(dest_ - orig)),
                    bounceCount(bounceCount_){
    }
    inline Ray(const Vec3Df& orig_, const Vec3Df& dest_) :
            orig(orig_), dest(dest_), dir(normal(dest_ - orig)){
    }
    inline Ray(const Vec3Df& orig_, const Vec3Df& dest_,
            const unsigned int bounceCount_) :
            orig(orig_), dest(dest_), dir(normal(dest_ - orig)),
                    bounceCount(bounceCount_){
    }
    inline virtual ~Ray(){
    }
	const Intersection intersect(const Triangle* const triangle);

	inline float intersect2(const Triangle* const triangle) const {
		const Vertex* vertices = triangle->vertices;
		const Vec3Df& v0 = vertices[0].p;
		const Vec3Df& v1 = vertices[1].p;
		const Vec3Df& v2 = vertices[2].p;

		const Vec3Df e1 = v1 - v0;
		const Vec3Df e2 = v2 - v0;

		const Vec3Df P = cross(dir, e2);
		const float det = dot(e1, P);

		if (abs(det) < 0.000000001)
			return 0;

		const float inv_det = 1.0f / det;

		const Vec3Df T = orig - vertices[0].p;

		const float u = dot(T, P) * inv_det;
		if (u < 0.0f || u > 1.0f)
			return 0;

		const Vec3Df Q = cross(T, e1);

		const float v = dot(dir, Q) *inv_det;
		if (v < 0.0f || u + v > 1.0f)
			return 0;

		const float t = dot(e2, Q) * inv_det;

		return t;
	}
};

#endif /* RAY_H */
