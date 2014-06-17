#ifndef RAYTRACING_H
#define RAYTRACING_H
#include <vector>
#include <GL/glut.h>
#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <cstdlib>

#include "Mesh.hpp"
#include "Ray.hpp"
#include "Image.hpp"
#include "OptionParser.hpp"

extern unsigned int texture;

extern Mesh MyMesh; //Main mesh;
extern std::vector<Vec3Df> MyLightPositions;
extern Vec3Df MyCameraPosition; //currCamera
extern unsigned int activeTexIndex;
extern unsigned int WindowSizeX; //window resolution width
extern unsigned int WindowSizeY; //window resolution height
extern unsigned int RayTracingResolutionX;  // largeur fenetre
extern unsigned int RayTracingResolutionY;  // largeur fenetre

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
    UNKNOWN, HELP, RAYTRACE, MESH
};
const option::Descriptor usage[] = {
        {UNKNOWN, 0, "", "", Arg::Unknown,
            "USAGE: computer_graphics [options]\n\nOptions:"},
        {HELP, 0, "h", "help", Arg::None,
            "--help, -h \tPrint usage and exit."},
        {RAYTRACE, 0, "r", "raytrace", Arg::None,
            "--raytrace, -r \tRay trace and exit."},
        {MESH, 0, "m", "mesh", Arg::Optional,
            "--mesh, -m \tMesh to load"},
        {UNKNOWN, 0, "" ,  "", Arg::None, "\nExamples:\n"
                "computer_graphics --trace\n"},
     {0,0,0,0,0,0}
};

//use this function for any preprocessing of the mesh.
int init(int argc, char ** argv);

//you can use this function to transform a click to an origin and destination
//the last two values will be changed. There is no need to define this function.
//it is defined elsewhere
void produceRay(int x_I, int y_I, Vec3Df & origin, Vec3Df & dest);
void produceRay(int x_I, int y_I, Vec3Df * origin, Vec3Df * dest);

void startRayTracing(int texIndex, bool verbose);

//your main function to rewrite
Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dest);

//a function to debug --- you can draw in OpenGL here
void yourDebugDraw();

//want keyboard interaction? Here it is...
void yourKeyboardFunc(char t, int x, int y);

void drawFPS();

#endif
