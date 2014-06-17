/*
 * Image.hpp
 *
 *  Created on: 12 Jun 2014
 *      Author: rick
 */

#ifndef IMAGE_H_
#define IMAGE_H_

/* fixed recursive includes */
class Image;

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include "Vec3D.hpp"

#define PIXEL_SIZE 3
#define COLOR_DEPTH 8

// image format
#define PNG 0
#define BMP 1
#define PPM 2

/* just add -D IMAGE_FORMAT=0 to the compiler to write png files */
#ifndef IMAGE_FORMAT
#define IMAGE_FORMAT BMP
#endif

#if IMAGE_FORMAT == PNG
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