/*
 * triangle.
 *
 *  Created on: 12 Jun 2014
 *      Author: rick
 */

#include "Triangle.hpp"

Triangle::Triangle(){
    vertices[0] = vertices[1] = vertices[2] = Vertex();
    calculateNormal();
}

Triangle::Triangle(const Triangle & triangle){
    vertices[0] = triangle.vertices[0];
    vertices[1] = triangle.vertices[1];
    vertices[2] = triangle.vertices[2];

    textures[0] = triangle.textures[0];
    textures[1] = triangle.textures[1];
    textures[2] = triangle.textures[2];

    calculateNormal();
}

Triangle::Triangle(const Vertex & v0, const Vertex & v1, const Vertex & v2,
        const Texture & t0, const Texture & t1, const Texture & t2){
    vertices[0] = v0;
    vertices[1] = v1;
    vertices[2] = v2;

    textures[0] = t0;   
    textures[1] = t1;
    textures[2] = t2;

    calculateNormal();
}

Triangle::Triangle(const Vertex & v0, const Vertex & v1, const Vertex & v2){
    vertices[0] = v0;
    vertices[1] = v1;
    vertices[2] = v2;
    calculateNormal();
}

Triangle & Triangle::operator=(const Triangle & triangle){
    vertices[0] = triangle.vertices[0];
    vertices[1] = triangle.vertices[1];
    vertices[2] = triangle.vertices[2];

    textures[0] = triangle.textures[0];
    textures[1] = triangle.textures[1];
    textures[2] = triangle.textures[2];

    calculateNormal();

    return (*this);
}
