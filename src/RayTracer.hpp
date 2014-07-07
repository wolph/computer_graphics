#ifndef RAYTRACING_H
#define RAYTRACING_H
#include <vector>
#include <GL/glut.h>
#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <cstdlib>
#include <thread>

#include "constants.hpp"
#include "Mesh.hpp"
#include "Image.hpp"
#include "OptionParser.hpp"
#include "Threading.hpp"
#include "Timer.hpp"
#include "Vec3D.hpp"
#include <algorithm>

extern unsigned int texture;

extern vector<Vec3Df> MyLightPositions;
extern unsigned int activeTexIndex;
extern unsigned int isDrawingTexture;
extern unsigned int isRealtimeRaytracing;

option::ArgStatus checkMeshArg(const option::Option& option, bool msg);

struct Arg: public option::Arg
{
  static void printError(const char* msg1, const option::Option& opt, const char* msg2)
  {
    fprintf(stderr, "%s", msg1);
    fwrite(opt.name, opt.namelen, 1, stderr);
    fprintf(stderr, "%s", msg2);
  }

  static option::ArgStatus Unknown(const option::Option& option, bool msg)
  {
    if (msg) printError("Unknown option '", option, "'\n");
    return option::ARG_ILLEGAL;
  }
};

enum optionIndex{
    UNKNOWN, HELP, RAYTRACE, SCENE, RAYTRACE_X, RAYTRACE_Y
};
const option::Descriptor usage[] = {
        {UNKNOWN, 0, "", "", Arg::Unknown,
            "USAGE: computer_graphics [options]\n\nOptions:"},
        {HELP, 0, "h", "help", Arg::None,
            "--help, -h \tPrint usage and exit."},
        {RAYTRACE, 0, "r", "raytrace", Arg::None,
            "--raytrace, -r \tRay trace and exit."},
        {SCENE, 0, "s", "scene", Arg::Optional,
            "--scene, -s \tScene to load"},
 		{RAYTRACE_X, 0, "x", "raytraceX", Arg::Optional,
				"--raytraceX, -x \tChoose raytracer resolution X."},
		{RAYTRACE_Y, 0, "y", "raytraceY", Arg::Optional,
				"--raytraceY, -y \tChoose raytracer resolution Y." },
        //{UNKNOWN, 0, "" ,  "", Arg::None, "\nExamples:\n" This example doesn't do anything...
        //        "computer_graphics --trace\n"},
		{ 0, 0, 0, 0, 0, 0 }
};

//use this function for any preprocessing of the mesh.
int init(int argc, char ** argv);

//you can use this function to transform a click to an origin and destination
//the last two values will be changed. There is no need to define this function.
//it is defined elsewhere
void produceRay(int x_I, int y_I, Vec3Df * origin, Vec3Df * dest);

void startRayTracing(int texIndex, bool needsRebuild=false);
void yourDebugDraw();

//your main function to rewrite
void performRayTracing(COLOR out, RAY ray, float importance, bool inside = false);

//want keyboard interaction? Here it is...
bool yourKeyboardPress(char t, int x, int y);
bool yourKeyboardRelease(char t, int x, int y);
void specialKeyboard(int key, int x, int y);
void specialKeyboardUp(int key, int x, int y);

void drawFPS();

inline Vec3Df pow(Vec3Df in1, float in2);

#endif
