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
            p(p){
    }
    inline Vertex(const Vertex & v) :
            p(v.p){
    }
    inline virtual ~Vertex(){
    }
    inline Vertex & operator=(const Vertex & v){
        p = v.p;
        return (*this);
    }
    Vec3Df p;
};

#endif // VERTEX_H
