/*
 * triangle.
 *
 *  Created on: 12 Jun 2014
 *      Author: rick
 */

#include "Triangle.hpp"

Triangle::Triangle(){
    vertices[0] = vertices[1] = vertices[2] = Vertex();
}

Triangle::Triangle(const Triangle & triangle){
    vertices[0] = triangle.vertices[0];
    vertices[1] = triangle.vertices[1];
    vertices[2] = triangle.vertices[2];

    textures[0] = triangle.textures[0];
    textures[1] = triangle.textures[1];
    textures[2] = triangle.textures[2];
}

Triangle::Triangle(const Vertex & v0, const Texture & t0, const Vertex & v1,
        const Texture & t1, const Vertex & v2, const Texture & t2){
    vertices[0] = v0;
    vertices[1] = v1;
    vertices[2] = v2;

    textures[0] = t0;
    textures[1] = t1;
    textures[2] = t2;
}

Triangle::~Triangle(){
}

Triangle & Triangle::operator=(const Triangle & triangle){
    vertices[0] = triangle.vertices[0];
    vertices[1] = triangle.vertices[1];
    vertices[2] = triangle.vertices[2];
    textures[0] = triangle.textures[0];
    textures[1] = triangle.textures[1];
    textures[2] = triangle.textures[2];
    return (*this);
}

void calculateNormal(){
//    Triangle triangle, Vertex n;
    /* triangle is the input triangle, n is the normal that will be filled as the output */
//    Vertex a, b;
//    a[X] = triangle[B][X] - triangle[A][X];
//    a[Y] = triangle[B][Y] - triangle[A][Y];
//    a[Z] = triangle[B][Z] - triangle[A][Z];
//
//    b[X] = triangle[C][X] - triangle[B][X];
//    b[Y] = triangle[C][Y] - triangle[B][Y];
//    b[Z] = triangle[C][Z] - triangle[B][Z];
//
//    n[X] = a[Y] * b[Z] - a[Z] * b[Y];
//    n[Y] = a[Z] * b[X] - a[X] * b[Z];
//    n[Z] = a[X] * b[Y] - a[Y] * b[X];
}
