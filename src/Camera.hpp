#pragma once
#include <GL/glut.h>
#include "Vec3D.hpp"
#include "constants.hpp"

class Camera {
public:
	float viewmat[16];
	float invmat[16];
	int forward;
	int side;
	int alt;
	int rotateX;
	int rotateY;

	Vec3Df pos;
	float xrot, yrot;

	Camera();
	void Update();
	void BuildMatrix();
	Vec3Df GetWorldPos(int px, int py);
};
