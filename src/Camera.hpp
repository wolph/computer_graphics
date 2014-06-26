#ifndef CAMERA
#define CAMERA
#include <GL/glut.h>
#include "Vec3D.hpp"
#include "constants.hpp"

class Camera {
public:
	double viewmat[16];
	double invmat[16];
	int forward;
	int side;
	int alt;

	Vec3Df pos;
	float xrot, yrot;

	Camera();
	void Update();
	void BuildMatrix();
	Vec3Df GetWorldPos(int px, int py);
};

#endif
