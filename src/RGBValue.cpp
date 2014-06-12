/*
 * RGBValue.cpp
 *
 *  Created on: 12 Jun 2014
 *      Author: rick
 */

#include "RGBValue.h"

RGBValue::RGBValue(float rI, float gI, float bI){
    r = rI;
    g = gI;
    b = bI;
}

float RGBValue::operator[](int i) const{
    switch(i){
        case 0:
            return r;
        case 1:
            return g;
        case 2:
            return b;
        default:
            return r;
    }
}
float & RGBValue::operator[](int i){
    switch(i){
        case 0:
            return r;
        case 1:
            return g;
        case 2:
            return b;
        default:
            return r;
    }
}
