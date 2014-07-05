#include <cstdio> /* file output */
#include <cstdint> /* ?? */
#include <cstdlib> /* ?? */
#include "Image.hpp"
using namespace std;

const char* image_exts[] = {"png", "bmp", "ppm"};

#if IMAGE_FORMAT == BMP
// convert value from little to big-endian
inline unsigned int htonl_(unsigned int n) {
	return
		(n & 0xFF000000 >> 24) |
		(n & 0xFF0000 >> 8) |
		(n & 0xFF00 << 8) |
		(n & 0xFF << 24);
}
#endif

bool Image::write(const char * filename2){
	string str = string(filename2) + string(".") + string(image_exts[IMAGE_FORMAT]);
	const char* filename = str.c_str();

    FILE* file;
    file = fopen(filename, "wb");
    if (!file) {
		printf("could not open file %s", filename);
        return false;
    }

#if IMAGE_FORMAT == PNG
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

    png_set_IHDR(png_ptr, info_ptr, width, height, COLOR_DEPTH,
            PNG_COLOR_TYPE_RGB,
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT);

    /* Initialize rows of PNG. */

    row_pointers = (png_voidp*)png_malloc(png_ptr,
            height * sizeof(png_byte *));
    int i = 0;
    for(y = 0;y < height;++y){
        png_byte *row = (png_byte*)png_malloc(png_ptr,
                sizeof(uint8_t) * width * PIXEL_SIZE);

#ifndef WIN32
        row_pointers[height - y - 1] = row;
#else
        row_pointers[y] = row;
#endif
        for(x = 0;x < width;++x){
            for(z = 0;z < PIXEL_SIZE;z++)
            *row++ = data[i++] * 255.0f;
        }
    }

    /* Write the image data to "fp". */
    png_init_io(png_ptr, file);
    png_set_rows(png_ptr, info_ptr, (png_bytepp)row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* The routine has successfully written the file, so we set
     "status" to a value which indicates success. */

    for(y = 0;y < height;y++){
        png_free(png_ptr, row_pointers[y]);
    }
    png_free(png_ptr, row_pointers);

#elif IMAGE_FORMAT == BMP

	// bitmap header
	unsigned char header[] = {
		0x42, 0x4D, /* "BM" */
		0x3E, 0xF6, 0x02, 0x00, /* filesize in bytes @ 2*/
		0x00, 0x00, 0x00, 0x00,
		0x36, 0x00, 0x00, 0x00,

		0x28, 0x00, 0x00, 0x00,
		0xFE, 0x00, 0x00, 0x00, /* width @ 18 */
		0xFE, 0x00, 0x00, 0x00, /* height @ 22 */
		0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0xF6, 0x02,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	// allocate buffer for converted image
	int len = width * height * 3;
	char* buf = new char[len];

	// get pointers to header
	unsigned int* filesize = (unsigned int*)(header + 2);
	unsigned int* w = (unsigned int*)(header + 18);
	unsigned int* h = (unsigned int*)(header + 22);

	// modify header
	*filesize = htonl_(sizeof(header) + len);
	*w = htonl_(width);
	*h = htonl_(height);

	// fill buffer, convert from RGB to BGR
	for (unsigned int i = 0; i < data.size() / 3; i++) {
		buf[i * 3 + 0] = (unsigned char)(data[i * 3 + 2] * 255.0f);
		buf[i * 3 + 1] = (unsigned char)(data[i * 3 + 1] * 255.0f);
		buf[i * 3 + 2] = (unsigned char)(data[i * 3 + 0] * 255.0f);
	}

	// write to file
	fwrite(header, 1, sizeof(header), file);
	fwrite(buf, 1, len, file);

	// deallocate buffer
	delete[] buf;
#else
	printf("Invalid image format!\n");
#endif

    fclose(file);
    printf("Image saved to %s!\n", filename);
    return true;
}
