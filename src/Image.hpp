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
	const unsigned int _width;
	const unsigned int _height;
	std::vector<float> _image;

	inline Image(const int width, const int height) :
		_width(width), _height(height), _image(3 * width * height, 0){}
	inline virtual ~Image(){};

	bool writeImage(const char * filename);
};

#endif /* IMAGE_H_ */
