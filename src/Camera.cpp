#include "Camera.hpp"
#include "matrix.hpp"
#include <cstring>

const static GLdouble identity2[] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
Camera::Camera() {
	memcpy(&viewmat, &identity2, sizeof(identity2));
	memcpy(&invmat, &identity2, sizeof(identity2));

	xrot = 0;
	yrot = 0;

	forward = 0;
	side = 0;
	rotateX = 0;
	rotateY = 0;
}

void Camera::Update() {
	Vec3Df dir1 = Vec3Df(cos((xrot + 90) / 180.0 * 3.14), 0, sin((xrot + 90) / 180.0 * 3.14));
	Vec3Df dir2 = Vec3Df(cos(xrot / 180.0 * 3.14), 0, sin(xrot / 180.0 * 3.14));
	dir1.normalize();
	dir2.normalize();
	pos += dir1 * 0.1 * forward;
	pos += dir2 * 0.1 * side;
	xrot += 2 * rotateX;
	yrot += 1 * rotateY;
	pos.p[Y] += alt * -0.1;

	if (pos.p[Y] > -0.01) pos.p[Y] = -0.01;
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
		dir.p[X] = mat[12] - 2;
		dir.p[Y] = mat[13];
		dir.p[Z] = mat[14];
		//dir.normalize();
		printf("%f %f %f\n", dir.p[X], dir.p[Y], dir.p[Z]);
	glPopMatrix();*/


	// translate to pos
	glRotated(xrot, 0, 1, 0);
	glRotated(yrot, cos(xrot / 180.0 * 3.14), 0, sin(xrot / 180.0 * 3.14));
	glTranslatef(pos.p[X], pos.p[Y], pos.p[Z]);

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
