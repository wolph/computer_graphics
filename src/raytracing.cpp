#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#include "raytracing.h"

//temporary variables
Vec3Df testRayOrigin;
Vec3Df testRayDestination;

//use this function for any preprocessing of the mesh.
void init(){
    //load the mesh file
    //feel free to replace cube by a path to another model
    //please realize that not all OBJ files will successfully load.
    //Nonetheless, if they come from Blender, they should.
    //there is a difference between windows written objs and Linux written objs.
    //hence, some models might not yet work well.
    //MyMesh.loadMesh("cube2.obj", true);

//    MyMesh.loadMesh("mesh/monkey.obj", true);
    MyMesh.loadMesh("mesh/dodgeColorTest.obj", true);

    //    MyMesh.loadMesh("cube.obj", true);
    MyMesh.computeVertexNormals();

    //one first move: initialize the first light source
    //at least ONE light source has to be in the scene!!!
    //here, we set it to the current location of the camera
    MyLightPositions.push_back(MyCameraPosition);
}

//return the color of your pixel.
Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dest){
    Vec3Df color = Vec3Df(1, 0, 0);
    color = performRayTracingArend(color, origin, dest);
    color = performRayTracingEwoud(color, origin, dest);
    color = performRayTracingLeon(color, origin, dest);
    color = performRayTracingMax(color, origin, dest);
    color = performRayTracingQu(color, origin, dest);
    color = performRayTracingRick(color, origin, dest);
    color = performRayTracingRutger(color, origin, dest);
    color = performRayTracingVince(color, origin, dest);
    color = performRayTracingYmte(color, origin, dest);
    return Vec3Df(1, 0, 0);
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
