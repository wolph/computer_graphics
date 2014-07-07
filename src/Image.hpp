#pragma once
#include <vector>
#include "Vec3D.hpp"
using namespace std;

#define PIXEL_SIZE 3
#define COLOR_DEPTH 8

// image format
#define PNG 0
#define BMP 1

/* just add -D IMAGE_FORMAT=0 to the compiler to write png files */
#ifndef IMAGE_FORMAT
#define IMAGE_FORMAT BMP
#endif

#if IMAGE_FORMAT == PNG
#include <png.h>
#endif

class Image {
public:
	int width;
    int height;
	vector<float> data;

	Image();
	Image(int width, int height);
	~Image();
	
	void load(char* path);
    bool write(const char * filename);
};
