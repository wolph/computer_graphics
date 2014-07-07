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

Texture2::Texture2(char* path) {
	FILE* fp = fopen(path, "rb");
	unsigned char* buf = new unsigned char[720 * 720 * 3];
	fseek(fp, 54, SEEK_SET);
	if (fread(buf, 1, 720 * 720 * 3, fp) != 720 * 720 * 3) {
		fclose(fp);
		throw("Error loading hardwood floor file.");
	}

	for (int i = 0; i < 720 * 720; i++){
		//hardwood[i * 3 + 0] = buf[i * 3 + 2] / 255.0f;
		//hardwood[i * 3 + 1] = buf[i * 3 + 1] / 255.0f;
		//hardwood[i * 3 + 2] = buf[i * 3 + 0] / 255.0f;
	}
	delete[] buf;
	fclose(fp);
}