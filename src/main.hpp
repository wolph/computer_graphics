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

#include "Vec3D.hpp"
#include <vector>
#include "Timer.hpp"

extern std::vector<Vec3Df> MyLightPositions;

// double buffered
extern unsigned int textures[2];
extern unsigned int activeTexIndex;
extern unsigned int isDrawingTexture;
extern unsigned int isRealtimeRaytracing;

void animate();
int main(int argc, char** argv);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void keyup(unsigned char key, int x, int y);

Timer fpsTimer(100);
char infoString[100];
void drawFPS();


#include "constants.hpp"
#include "mouse.hpp"
#include "RayTracer.hpp"

#endif /* MAIN_HPP_ */
