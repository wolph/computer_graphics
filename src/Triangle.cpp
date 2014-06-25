/*
 * triangle.
 *
 *  Created on: 12 Jun 2014
 *      Author: rick
 */

#include "Triangle.hpp"

Triangle::Triangle(const Triangle & triangle) : material(triangle.material){
    vertices[0] = triangle.vertices[0];
    vertices[1] = triangle.vertices[1];
    vertices[2] = triangle.vertices[2];

    textures[0] = triangle.textures[0];
    textures[1] = triangle.textures[1];
	textures[2] = triangle.textures[2];

	calculateNormal();
}

Triangle::Triangle(const Vertex & v0, const Vertex & v1, const Vertex & v2,
                   const Texture & t0, const Texture & t1, const Texture & t2, const Material & m) : material(m){
    if(&v0 != 0)
        vertices[0] = v0;
    if(&v1 != 0)
        vertices[1] = v1;
    if(&v2 != 0)
        vertices[2] = v2;

    if(&t0 != 0)
        textures[0] = t0;
    if(&t1 != 0)
        textures[1] = t1;
    if(&t2 != 0)
		textures[2] = t2;

	calculateNormal();
}

Triangle::Triangle(const Vertex & v0, const Vertex & v1, const Vertex & v2, const Material& m) : material(m){
    if(&v0 != 0)
        vertices[0] = v0;
    if(&v1 != 0)
        vertices[1] = v1;
    if(&v2 != 0)
		vertices[2] = v2;

	calculateNormal();
}

Triangle::~Triangle(){
}


void Triangle::calculateNormal(){
	normal = vertices[0].position.crossProduct(vertices[1].position - vertices[0].position,
		vertices[2].position - vertices[0].position);
	normal.normalize();
}