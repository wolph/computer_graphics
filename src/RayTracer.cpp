#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#include "RayTracer.hpp"

//temporary variables
Vec3Df testRayOrigin;
Vec3Df testRayDestination;
string input;

//use this function for any preprocessing of the mesh.
void init(int argc, char **argv){
    //load the mesh file
    //feel free to replace cube by a path to another model
    //please realize that not all OBJ files will successfully load.
    //Nonetheless, if they come from Blender, they should.
    //there is a difference between windows written objs and Linux written objs.
    //hence, some models might not yet work well.

    RayTracingResolutionX = 1000; // These resolutions should be the same as the window,
    RayTracingResolutionY = 1000; // otherwise unexpected behaviour occurs.

    if(argc == 2){
        input = argv[1];
    }else{
        cout << "Mesh file name: (0: cube, 1: monkey, 2: dodgeColorTest)"
                << endl << "You can omit the mesh/ path and the .obj extension."
                << endl;
        cin >> input;
    }

	if (input == "0")
		input = "cube";
	else if (input == "1")
	input = "monkey";
	else if (input == "2")
		input = "dodgeColorTest";
	input = string("mesh/").append(input).append(".obj");
    MyMesh.loadMesh(input.c_str(), true);

    MyMesh.computeVertexNormals();

    //one first move: initialize the first light source
    //at least ONE light source has to be in the scene!!!
    //here, we set it to the current location of the camera
    MyLightPositions.push_back(MyCameraPosition);
}

//return the color of your pixel.
Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dest){
    Ray ray = Ray(origin, dest);
    //ray = performRayTracingArend(ray);
    //ray = performRayTracingEwoud(ray);
    //ray = performRayTracingLeon(ray);
    ray = performRayTracingMax(ray);
    //ray = performRayTracingQu(ray);
    //ray = performRayTracingRick(ray);
    //ray = performRayTracingRutger(ray);
    //ray = performRayTracingVince(ray);
    //ray = performRayTracingYmte(ray);
    return ray.getColor();
}

void yourDebugDraw(){
    //draw open gl debug stuff
    //this function is called every frame

    //as an example:
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glColor3f(1, 0, 1);
    glBegin(GL_LINES);
    glVertex3f(testRayOrigin[0], testRayOrigin[1], testRayOrigin[2]);
    glVertex3f(testRayDestination[0], testRayDestination[1],
            testRayDestination[2]);
    glEnd();
    glPointSize(10);
    glBegin(GL_POINTS);
    glVertex3fv(MyLightPositions[0].pointer());
    glEnd();
    glPopAttrib();

}

void yourKeyboardFunc(char t, int x, int y){
    // do what you want with the keyboard input t.
    // x, y are the screen position

    //here I use it to get the coordinates of a ray, which I then draw in the debug function.
    produceRay(x, y, testRayOrigin, testRayDestination);

    std::cout << t << " pressed! The mouse was in location " << x << "," << y
            << "!" << std::endl;
}
