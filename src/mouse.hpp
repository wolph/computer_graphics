/** \file traqueboule.hpp
 
 Utilitaires pour appliquer une traqueboule a une fenetre OpenGL.
 
 */
#pragma once
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#include <math.h>
#include "matrix.hpp"
#include "stdio.h"
#include "Vec3D.hpp"
#include "Scene.hpp"
#include "Timer.hpp"
#include "program.hpp"

static const float speedfact = 0.2f;
extern Program* prog;

/** votre fonction d'affichage */
void display();

/** Placement de la scene par rapport a la camera */
GLdouble view_matrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
GLdouble view_inverse[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

/** Gestion de la souris */
int oldMouseX, oldMouseY, rotatingXY = 0, translateXY = 0, moveZ = 0;
bool leftButton, rightButton;
double angleX, angleY;
bool mouseMode = false;
Timer startDrag(1);

/** Mouse button click */
void mouseFunc(int button, int state, int x, int y){ 
    double last_click = startDrag.next().count();
    if(button == GLUT_LEFT_BUTTON && state == GLUT_UP && last_click < 0.3){
        mouseMode = !mouseMode;
        if(mouseMode)
	        glutWarpPointer(WINDOW_RES_X / 2, WINDOW_RES_Y / 2);
    }

    // press left
	oldMouseX = x;
	oldMouseY = y;
	switch(button){
	    case GLUT_LEFT_BUTTON:
	        leftButton = state == GLUT_DOWN;
	        break;
	    case GLUT_RIGHT_BUTTON:
	        rightButton = state == GLUT_DOWN;
	        break;
	}
}

/** Mouse moded */
void mouseMotionFunc(int x, int y){
    if(mouseMode){
        /* Don't move if there's no movement */
        if(abs(x - WINDOW_RES_X / 2 + y - WINDOW_RES_Y / 2) < 2)return;
    }else if(!leftButton){
        return;
    }

	double dx, dy, nrm;

	// move cam around
	if(mouseMode){
	    dx = x - WINDOW_RES_X / 2;
	    dy = y - WINDOW_RES_Y / 2;
    }else{
	    dx = x - oldMouseX;
	    dy = y - oldMouseY;
    }
	nrm = sqrt(dx * dx + dy * dy + dx * dx + dy * dy) * 0.002;// speedfact;

	prog->scene.cam.xrot += dx * nrm;
	prog->scene.cam.yrot += dy * nrm;

	if (prog->scene.cam.yrot < -90)
		prog->scene.cam.yrot = -90;
	if (prog->scene.cam.yrot > 90)
		prog->scene.cam.yrot = 90;

    if(mouseMode)
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
