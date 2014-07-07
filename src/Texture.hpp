#pragma once
#include "Vec3D.hpp"

typedef Vec3Df Texture;

class Texture2 {
public:
	char* data;

	Texture2(char* path);
	~Texture2();
};

void drawTexture(int texIndex);