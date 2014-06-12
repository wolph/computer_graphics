/*
 * Image.cpp
 *
 *  Created on: 12 Jun 2014
 *      Author: rick
 */

#include "Image.h"

Image::Image(int width, int height) :
        _width(width), _height(height){
    _image.resize(3 * _width * _height);
}

Image::~Image(){
}

void Image::setPixel(int i, int j, float r, float g, float b){
    int k = _width * j + i;
    _image[3 * (k++)] = r;
    _image[3 * (k++)] = g;
    _image[3 * (k++)] = b;
}

void Image::setPixel(int i, int j, Vec3Df rgb){
    int k = _width * j + i;
    for(int l = 0;l < PIXEL_SIZE;l++)
        _image[3 * (k++)] = rgb[l];
}

bool Image::writeImage(const char * filename){
    FILE* file;
    file = fopen(filename, "wb");
    if(!file){
        printf("dump file problem... file\n");
        return false;
    }

#ifdef PNG
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y, z;
    png_voidp* row_pointers = NULL;

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(png_ptr == NULL){
        png_destroy_write_struct(&png_ptr, &info_ptr);
    }

    info_ptr = png_create_info_struct(png_ptr);
    if(info_ptr == NULL){
        png_destroy_write_struct(&png_ptr, &info_ptr);
    }

    /* Set up error handling. */

    if(setjmp(png_jmpbuf(png_ptr))){
        png_destroy_write_struct(&png_ptr, &info_ptr);
    }

    /* Set image attributes. */

    png_set_IHDR(png_ptr, info_ptr, _width, _height, COLOR_DEPTH,
    PNG_COLOR_TYPE_RGB,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT);

    /* Initialize rows of PNG. */

    row_pointers = (png_voidp*)png_malloc(png_ptr,
            _height * sizeof(png_byte *));
    int i = 0;
    for(y = 0;y < _height;++y){
        png_byte *row = (png_byte*)png_malloc(png_ptr,
                sizeof(uint8_t) * _width * PIXEL_SIZE);

        row_pointers[y] = row;
        for(x = 0;x < _width;++x){
            for(z = 0;z < PIXEL_SIZE;z++)
                *row++ = _image[i++] * 255.0f;
        }
    }

    /* Write the image data to "fp". */
    png_init_io(png_ptr, file);
    png_set_rows(png_ptr, info_ptr, (png_bytepp)row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* The routine has successfully written the file, so we set
     "status" to a value which indicates success. */

    for(y = 0;y < _height;y++){
        png_free(png_ptr, row_pointers[y]);
    }
    png_free(png_ptr, row_pointers);

#else
    fprintf(file, "P6\n%i %i\n255\n", _width, _height);

    std::vector<unsigned char> imageC(_image.size());

    for(unsigned int i = 0;i < _image.size();++i)
    imageC[i] = (unsigned char)(_image[i] * 255.0f);

    int t = fwrite(&(imageC[0]), _width * _height * PIXEL_SIZE, 1, file);
    if(t != 1){
        printf("Dump file problem... fwrite\n");
        return false;
    }
#endif

    fclose(file);
    return true;
}
