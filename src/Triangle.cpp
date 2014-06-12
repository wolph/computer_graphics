/*
 * triangle.
 *
 *  Created on: 12 Jun 2014
 *      Author: rick
 */

#include "Triangle.hpp"

Triangle::Triangle(){
    v[0] = v[1] = v[2] = 0;
}

Triangle::Triangle(const Triangle & t2){
    v[0] = t2.v[0];
    v[1] = t2.v[1];
    v[2] = t2.v[2];

    t[0] = t2.t[0];
    t[1] = t2.t[1];
    t[2] = t2.t[2];

}

Triangle::Triangle(unsigned int v0, unsigned int t0, unsigned int v1,
        unsigned int t1, unsigned int v2, unsigned int t2){
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;

    t[0] = t0;
    t[1] = t1;
    t[2] = t2;
}

Triangle::~Triangle(){
}

Triangle & Triangle::operator=(const Triangle & t2){
    v[0] = t2.v[0];
    v[1] = t2.v[1];
    v[2] = t2.v[2];
    t[0] = t2.v[0];
    t[1] = t2.v[1];
    t[2] = t2.v[2];
    return (*this);
}
