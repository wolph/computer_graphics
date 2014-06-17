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

inline const Vec3Df normal(const Vec3Df& in){
    /* WARNING! This is actually unsafe, but it's fast so who cares ;) */
    Vec3Df res = Vec3Df(in);
    res.normalize();
    return res;
}

class Ray{
public:
    Vec3Df color;
    const Vec3Df orig;
    const Vec3Df dest;
    const Vec3Df dir;

//public:
    //Ray(Vec3Df& color, const Vec3Df& orig, const Vec3Df& dest);
    inline Ray(Vec3Df& color_, const Vec3Df& orig_, const Vec3Df& dest_) :
            color(color_), orig(orig_), dest(dest_), dir(normal(dest_ - orig)){
    }
    virtual ~Ray();
    bool intersect(Vec3Df& triangle);
    const Vec3Df& getColor() const;
    void setColor(const Vec3Df& color = Vec3Df(1, 1, 1));
    void setColor(const float red, const float green, const float blue);
};

#endif /* RAY_H */
