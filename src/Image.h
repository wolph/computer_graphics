/*
 * Image.h
 *
 *  Created on: 12 Jun 2014
 *      Author: rick
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include "Vec3D.h"

#define PIXEL_SIZE 3
#define COLOR_DEPTH 8

#ifdef PNG
#include <png.h>
#endif

class Image{
public:
    unsigned int _width;
    unsigned int _height;
    std::vector<float> _image;

    Image(int width, int height);
    virtual ~Image();

    void setPixel(int i, int j, const Vec3Df rgb);
    void setPixel(int i, int j, const float r, const float g, const float b);
    bool writeImage(const char * filename);
};

#endif /* IMAGE_H_ */
