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
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "Mesh.hpp"
#include "Ray.hpp"
#include "Image.hpp"
#include "traqueboule.hpp"
#include "RayTracer.hpp"

Vec3Df MyCameraPosition;

std::vector<Vec3Df> MyLightPositions;

Mesh MyMesh; //Main mesh

// Utilisé pour essayer différents types de rendu
// Utilisé via le paramètre "-t" en ligne de commande
enum {
    TRIANGLE = 0, MODEL = 1
};
unsigned int type = MODEL;

unsigned int WindowSize_X = 1000;  // largeur fenetre
unsigned int WindowSize_Y = 1000;  // hauteur fenetre

unsigned int RayTracingResolutionX = 400;  // largeur fenetre
unsigned int RayTracingResolutionY = 400;  // largeur fenetre

void dessinerRepere(float length);
void dessiner();
void animate();
int main(int argc, char** argv);
void display(void);
void reshape(int w, int h);
void produceRay(int x_I, int y_I, Vec3Df * origin, Vec3Df * dest);
void produceRay(int x_I, int y_I, Vec3Df & origin, Vec3Df & dest);
void keyboard(unsigned char key, int x, int y);

#endif /* MAIN_HPP_ */
