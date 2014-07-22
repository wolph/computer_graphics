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
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// translate to pos
	glRotatef(xrot, 0, 1, 0);
	glRotatef(yrot, cos(xrot / 180.0 * 3.14), 0, sin(xrot / 180.0 * 3.14));
	glTranslatef(pos.p[X], pos.p[Y], pos.p[Z]);

	glGetFloatv(GL_MODELVIEW_MATRIX, viewmat);
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
