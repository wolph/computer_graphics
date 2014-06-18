/*
 * main.hpp
 *
 *  Created on: 16 Jun 2014
 *      Author: rick
 */

#ifndef MAIN_HPP_CG
#define MAIN_HPP_CG

#ifdef WIN32
#include <windows.h>
#endif
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <GL/glut.h>
#include <assert.h>
#include <thread>

#include "Mesh.hpp"
#include "Tree.hpp"
#include "Ray.hpp"
#include "Image.hpp"
#include "traqueboule.hpp"
#include "RayTracer.hpp"

extern Vec3Df MyCameraPosition;

extern std::vector<Vec3Df> MyLightPositions;

// double buffered
extern unsigned int textures[2];
extern unsigned int activeTexIndex;
extern unsigned int isDrawingTexture;
extern unsigned int isRealtimeRaytracing;
extern Mesh MyMesh; //Main mesh
extern Tree MyTree;

// Utilisé pour essayer différents types de rendu
// Utilisé via le paramètre "-t" en ligne de commande
enum {
    TRIANGLE = 0, MODEL = 1
};
unsigned int type = MODEL;

extern unsigned int WindowSize_X;  // largeur fenetre
extern unsigned int WindowSize_Y;  // hauteur fenetre

extern unsigned int RayTracingResolutionX;  // largeur fenetre
extern unsigned int RayTracingResolutionY;  // largeur fenetre

extern unsigned int previewResX;
extern unsigned int previewResY;

extern unsigned int numThreads;
extern unsigned int msaa;

void drawAxes(float length);
void draw();
void animate();
int main(int argc, char** argv);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);

#endif /* MAIN_HPP_ */
