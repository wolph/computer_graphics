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
    Vec3Df res(in);
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
	inline float intersect(const Triangle* const triangle) const {
		const Vertex* vertices = triangle->vertices;
		const Vec3Df& v0 = vertices[0].p;
		const Vec3Df& v1 = vertices[1].p;
		const Vec3Df& v2 = vertices[2].p;

		Vec3Df e1 = v1;
		e1 -= v0;
		Vec3Df e2 = v2;
		e2 -= v0;

		const Vec3Df P = cross(dir, e2);
		const float det = dot(e1, P);

		if (det < 0.000000001 && det > -0.000000001)
			return 0;

		float inv_det = 1.0f;
		inv_det /= det;

		Vec3Df T = orig;
		T -= vertices[0].p;

		float u = dot(T, P);
		u *= inv_det;
		if (u < 0.0f || u > 1.0f)
			return 0;

		const Vec3Df Q = cross(T, e1);

		float v = dot(dir, Q);
		v *= inv_det;
		if (v < 0.0f || u + v > 1.0f)
			return 0;

		float t = dot(e2, Q);
		t *= inv_det;

		return t;
	}
};

#endif /* RAY_H */
