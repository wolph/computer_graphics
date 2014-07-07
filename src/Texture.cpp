#include "Texture.hpp"
#include <GL/glut.h>

extern unsigned int textures[2];

void drawTexture(int texIndex) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[texIndex]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(0, 0, 0);
	glTexCoord2f(1, 0);
	glVertex3f(4, 0, 0);
	glTexCoord2f(1, 1);
	glVertex3f(4, 4, 0);
	glTexCoord2f(0, 1);
	glVertex3f(0, 4, 0);
	glEnd();
}

Texture::Texture() {
}

void Texture::init() {
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (glGetError() != GL_NO_ERROR)
		throw string("failed to create texture");
}

Texture::~Texture() {
	glDeleteTextures(1, &id);
}

/**
* draw a full-screen texture
*/
void Texture::draw() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, id);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(0, 0, 0);
	glTexCoord2f(1, 0);
	glVertex3f(4, 0, 0);
	glTexCoord2f(1, 1);
	glVertex3f(4, 4, 0);
	glTexCoord2f(0, 1);
	glVertex3f(0, 4, 0);
	glEnd();
}

void Texture::upload(Image& image) {
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, GL_RGB,
		GL_FLOAT, &image.data[0]);
}