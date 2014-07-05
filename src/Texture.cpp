/*
 * Texture.cpp
 *
 *  Created on: 16 Jun 2014
 *      Author: rick
 */

#include "Texture.hpp"
#include <GL/glut.h>

/**
* draw a full-screen texture
*/
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
