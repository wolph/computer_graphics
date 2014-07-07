#pragma once
#include "Vec3D.hpp"

typedef Vec3Df Texture;

class Texture {
public:
	char* data;

	Texture(char* path);
	~Texture();
};

void drawTexture(int texIndex);