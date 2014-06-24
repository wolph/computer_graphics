#ifndef VERTEX_H
#define VERTEX_H

/* fixed recursive includes */
class Vertex;

#include "Vec3D.hpp"

/************************************************************
 * Vertex of the mesh (containing normal n and vertex v)
 ************************************************************/
class Vertex{
public:
    inline Vertex(){
    }
    inline Vertex(const Vec3Df & p) :
            position(p){
    }
    inline Vertex(const Vec3Df & p, const Vec3Df & n) :
            position(p), normal(n){
    }
    inline Vertex(const Vertex & v) :
            position(v.position), normal(v.normal){
    }
    inline virtual ~Vertex(){
    }
    Vec3Df position;
    Vec3Df normal;
};

#endif // VERTEX_H
