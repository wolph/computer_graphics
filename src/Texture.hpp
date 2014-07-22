#pragma once
#include "Image.hpp"
#include "Vec3D.hpp"

class Texture {
public:
	unsigned int id;

	Texture();
	~Texture();

	void init();
	void upload(Image& image);
	void draw();
};