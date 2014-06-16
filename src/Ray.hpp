/*
 * Ray.hpp
 *
 *  Created on: 11 Jun 2014
 *      Author: rick
 */

#ifndef RAY_H
#define RAY_H

#include "Vec3D.hpp"
#include "Mesh.hpp"

class Ray{
private:
    Vec3Df orig;
	Vec3Df dir;
    Vec3Df dest;
    Vec3Df color;

public:
    Ray();
    Ray(const Vec3Df orig, const Vec3Df dest);
    Ray(Vec3Df color, const Vec3Df orig, const Vec3Df dest);
    virtual ~Ray();
    bool intersect(Vec3Df triangle);
    const Vec3Df& getColor() const;
    void setColor(const Vec3Df& color = Vec3Df(0, 1, 0));
	const Vec3Df& getDest() const;
    void setDest(const Vec3Df& dest);
	const Vec3Df& getDir() const;
	void setDir(const Vec3Df& dir);
    const Vec3Df& getOrig() const;
    void setOrig(const Vec3Df& orig);
};

#endif /* RAY_H */
