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
        cout << "Mesh file name: (0: monkey, 1: cube, 2: dodgeColorTest, 3: simple_monkey)"
                << endl << "You can omit the mesh/ path and the .obj extension."
                << endl;
        cin >> input;
    }

	if (input == "0")
		input = "monkey";
	else if (input == "1")
	    input = "cube";
	else if (input == "2")
		input = "dodgeColorTest";
	else if (input == "3")
	    input = "simple_monkey";

	input = string("mesh/").append(input).append(".obj");
    MyMesh.loadMesh(input.c_str(), true);

    MyMesh.computeVertexNormals();

    //one first move: initialize the first light source
    //at least ONE light source has to be in the scene!!!
    //here, we set it to the current location of the camera
    MyLightPositions.push_back(MyCameraPosition);
}


#define dot Vec3Df::dotProduct
#define cross Vec3Df::crossProduct

#define v0 t.vertices[0].p
#define v1 t.vertices[1].p
#define v2 t.vertices[2].p

#define rayOrig ray.orig
#define rayDir ray.dir
#define VEWY_HIGH 10e6f

/** Calculate intersection between triangle and ray */
float intersect(const Triangle& t, const Ray& ray){

	float angle = dot(t.normal, rayDir); // The cosine of the angle of the vectors (dotproduct of the vectors).

	/* Floats are only rarely exactly 0, are you sure this is correct? */
	if (angle == 0) // If the ray and the plane are parallel (so their angle is 0), they won't intersect.
		return VEWY_HIGH;

	float rayD = -(dot(t.normal, rayOrig) + dot(t.normal, v0)) / angle; // The distance of the ray's origin
	// to the plane in which lies the triangle.

	Vec3Df rayHit = rayOrig + rayD * rayDir; // The intersection point of the ray and the plane in which lies the triangle.

	if (dot(t.normal, cross(v1 - v0, rayHit - v0)) < 0)
		return VEWY_HIGH;
	if (dot(t.normal, cross(v2 - v1, rayHit - v1)) < 0)
		return VEWY_HIGH;
	if (dot(t.normal, cross(v0 - v2, rayHit - v2)) < 0)
		return VEWY_HIGH;

	return rayD;
}

//return the color of your pixel.
const Vec3Df& performRayTracing(const Vec3Df & origin, const Vec3Df & dest){
    Vec3Df color = Vec3Df(1, 1, 1);
    Ray ray = Ray(color, origin, dest);

	/* Actual ray tracing code */
	float hit = VEWY_HIGH;
	unsigned int amountOfTriangles = MyMesh.triangles.size();
	for (unsigned int i = 0; i < amountOfTriangles; i++){
		float ins = intersect(MyMesh.triangles[i], ray);
		if (ins < hit && ins > 0)
			hit = ins;
	}
	//hit = 1 / ((hit * 2) + 1); // Arithmetic function for getting a usable color.
	ray.setColor(Vec3Df(hit, hit / 5, hit * 5));

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
