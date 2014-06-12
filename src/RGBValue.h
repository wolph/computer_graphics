/*
 * RGBValue.h
 *
 *  Created on: 12 Jun 2014
 *      Author: rick
 */

#ifndef RGBVALUE_H_
#define RGBVALUE_H_

class RGBValue{
public:
    RGBValue();
    float r;
    float b;
    float g;
    virtual ~RGBValue();

    inline RGBValue(float rI = 0, float gI = 0, float bI = 0);
    float operator[](int i) const;
    float & operator[](int i);
};

#endif /* RGBVALUE_H_ */
