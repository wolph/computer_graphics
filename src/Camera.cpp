#include "Camera.hpp"
#include "matrix.hpp"
#include <cstring>

Camera::Camera() {
	memcpy(&viewmat, &identity, sizeof(identity));
	memcpy(&invmat, &identity, sizeof(identity));

	xrot = 0;
	yrot = 0;

	forward = 0;
	side = 0;
}

void Camera::Update() {
	Vec3Df dir1 = Vec3Df(cos((xrot + 90) / 180.0 * 3.14), 0, sin((xrot + 90) / 180.0 * 3.14));
	Vec3Df dir2 = Vec3Df(cos(xrot / 180.0 * 3.14), 0, sin(xrot / 180.0 * 3.14));
	dir1.normalize();
	dir2.normalize();
	pos += dir1 * 0.1 * forward;
	pos += dir2 * 0.1 * side;
	pos.p[1] += alt * -0.1;

	if (pos.p[1] > -0.01) pos.p[1] = -0.01;
}

void Camera::BuildMatrix() {
	//*(Vec3Df*)&viewmat[12] = pos;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// get direction
	/*glPushMatrix();
		glTranslated(1, 0, 0);
		//glRotated(xrot, 0, 1, 0);
		glTranslated(1, 0, 0);
		glRotated(yrot, cos(xrot / 180.0 * 3.14), 0, sin(xrot / 180.0 * 3.14));
		glTranslated(1, 0, 0);
		double mat[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, mat);
		dir.p[0] = mat[12] - 2;
		dir.p[1] = mat[13];
		dir.p[2] = mat[14];
		//dir.normalize();
		printf("%f %f %f\n", dir.p[0], dir.p[1], dir.p[2]);
	glPopMatrix();*/


	// translate to pos
	glRotated(xrot, 0, 1, 0);
	glRotated(yrot, cos(xrot / 180.0 * 3.14), 0, sin(xrot / 180.0 * 3.14));
	glTranslatef(pos.p[0], pos.p[1], pos.p[2]);

	glGetDoublev(GL_MODELVIEW_MATRIX, viewmat);
}

Vec3Df Camera::GetWorldPos(int px, int py) {
	double mv[16];
	double pr[16];
	int vp[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, mv);
	glGetDoublev(GL_PROJECTION_MATRIX, pr);
	glGetIntegerv(GL_VIEWPORT, vp);

	double x, y, z;
	gluUnProject(double(px), double(py), 0, mv, pr, vp, &x, &y, &z);

	return Vec3Df(x, y, z);
}