#pragma once
#include <vector>
#include "Vec3D.hpp"
using namespace std;

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

class Image {
public:
	const unsigned int width;
    const unsigned int height;
	vector<float> data;

    inline Image(const int width, const int height) :
        width(width), height(height), data(3 * width * height, 0) {}
    inline virtual ~Image(){};

    bool write(const char * filename);
};
