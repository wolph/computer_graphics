/** \file traqueboule.hpp
 
 Utilitaires pour appliquer une traqueboule a une fenetre OpenGL.
 
 */
#ifndef TRAQUEBOULE
#define TRAQUEBOULE
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#include <math.h>
#include "matrix.hpp"
#include "stdio.h"
#include "Vec3D.hpp"
#include "Scene.hpp"

extern Scene MyScene;

static const float speedfact = 0.2f;

/** votre fonction d'affichage */
void display();

/** Placement de la scene par rapport a la camera */
GLdouble view_matrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
GLdouble view_inverse[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

/** Gestion de la souris */
int oldMouseX, oldMouseY, rotatingXY = 0, translateXY = 0, moveZ = 0;
bool leftButton, rightButton;
double angleX, angleY;


/** Initialize model view matrix */
void initViewTransform() {
}

/** Tranform view */
void viewTransform() {
	MyScene.cam.BuildMatrix();
    glMultMatrixd(MyScene.cam.viewmat);
}

/** Mouse button click */
void mouseFunc(int button, int state, int x, int y){ 
    // press left
	oldMouseX = x;
	oldMouseY = y;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		leftButton = true;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		leftButton = false;
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		rightButton = true;
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
		rightButton = false;
}

/** Mouse moded */
void mouseMotionFunc(int x, int y){
	if (abs(x - WINDOW_RES_X/2 + y - WINDOW_RES_Y/2) < 2)
		return;

	double dx, dy, nrm;

	// move cam around
	dx = x - WINDOW_RES_X / 2;
	dy = y - WINDOW_RES_Y / 2;

	nrm = sqrt(dx * dx + dy * dy + dx * dx + dy * dy) * 0.002;// speedfact;

	MyScene.cam.xrot += dx * nrm;
	MyScene.cam.yrot += dy * nrm;

	if (MyScene.cam.yrot < -90) MyScene.cam.yrot = -90;
	if (MyScene.cam.yrot > 90) MyScene.cam.yrot = 90;

	glutWarpPointer(WINDOW_RES_X / 2, WINDOW_RES_Y / 2);

	oldMouseX = x;
	oldMouseY = y;

	glutPostRedisplay();
}

/** Rotate mouse */
void mouseRotate(double angle, double x, double y, double z){
    double tx, ty, tz;

    tx = view_matrix[12];
    view_matrix[12] = 0;
    ty = view_matrix[13];
    view_matrix[13] = 0;
    tz = view_matrix[14];
    view_matrix[14] = 0;

    glLoadIdentity();
    glRotatef(angle, x, y, z);
    glMultMatrixd(view_matrix);
    glGetDoublev(GL_MODELVIEW_MATRIX, view_matrix);

    view_matrix[12] = tx;
    view_matrix[13] = ty;
    view_matrix[14] = tz;

    inverse(view_matrix, view_inverse);
    glutPostRedisplay();
}

/// Projection dans le repere du monde
void mouseProject(const GLdouble *m, const GLdouble* p, GLdouble* q){
    double pp[4];
    //cout<<"tb, matrix: "; printMatrix(view_matrix); cout<<endl;
    //cout<<"tb, inverse: "; printMatrix(view_inverse); cout<<endl;
    project(m, p, pp);
    //cout<<"proj: "<<pp[0]<<", "<<pp[1]<<", "<<pp[2]<<", "<<pp[3]<<endl;
    project(view_inverse, pp, q);
    //cout<<"projRep: "<<q[0]<<", "<<q[1]<<", "<<q[2]<<", "<<q[3]<<endl;
}

void mouseProject(const GLdouble* p, GLdouble* q){
    //cout<<"proj: "<<pp[0]<<", "<<pp[1]<<", "<<pp[2]<<", "<<pp[3]<<endl;
    project(view_inverse, p, q);
    //cout<<"projRep: "<<q[0]<<", "<<q[1]<<", "<<q[2]<<", "<<q[3]<<endl;
}
Vec3Df getCameraPosition(){
    const GLdouble p[] = {0, 0, 0, 1};
    GLdouble LightP[4];
    mouseProject(p, LightP);
    Vec3Df LightPos;
    LightPos[0] = LightP[0];
    LightPos[1] = LightP[1];
    LightPos[2] = LightP[2];
    return LightPos;
}

Vec3Df getWorldPositionOfPixel(unsigned int px, unsigned int py){

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
#endif
