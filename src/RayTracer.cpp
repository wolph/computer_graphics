#include "RayTracer.hpp"
#include <ctime>

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
        cout
                << "Mesh file name: (0: monkey, 1: cube, 2: dodgeColorTest, 3: simple_monkey)"
                << endl << "You can omit the mesh/ path and the .obj extension."
                << endl;
        cin >> input;
    }

    if(input == "0")
        input = "cube";
    else if(input == "1")
        input = "simple_monkey";
    else if(input == "2")
        input = "monkey";
    else if(input == "3")
        input = "dodgeColorTest";

    input = string("mesh/").append(input).append(".obj");
    MyMesh.loadMesh(input.c_str(), true);

    MyMesh.computeVertexNormals();

    //one first move: initialize the first light source
    //at least ONE light source has to be in the scene!!!
    //here, we set it to the current location of the camera
    MyLightPositions.push_back(MyCameraPosition);
}


//transformer le x, y en position 3D
void produceRay(int x_I, int y_I, Vec3Df * origin, Vec3Df * dest){
    int viewport[4];
    double modelview[16];
    double projection[16];
    //point sur near plane
    //double positionN[3];
    //point sur far plane
    //double positionF[3];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview); //recuperer matrices
    glGetDoublev(GL_PROJECTION_MATRIX, projection); //recuperer matrices
    glGetIntegerv(GL_VIEWPORT, viewport); //viewport
    int y_new = viewport[3] - y_I;

    double x, y, z;

    gluUnProject(x_I, y_new, 0, modelview, projection, viewport, &x, &y, &z);
    origin->p[0] = float(x);
    origin->p[1] = float(y);
    origin->p[2] = float(z);
    gluUnProject(x_I, y_new, 1, modelview, projection, viewport, &x, &y, &z);
    dest->p[0] = float(x);
    dest->p[1] = float(y);
    dest->p[2] = float(z);
}

void produceRay(int x_I, int y_I, Vec3Df & origin, Vec3Df & dest){
    produceRay(x_I, y_I, &origin, &dest);
}

void startRayTracing(){
    //C'est nouveau!!!
    //commencez ici et lancez vos propres fonctions par rayon.

    cout << "Raytracing" << endl;

    Image result(RayTracingResolutionX, RayTracingResolutionY);
    Vec3Df origin00, dest00;
    Vec3Df origin01, dest01;
    Vec3Df origin10, dest10;
    Vec3Df origin11, dest11;
    Vec3Df origin, dest;

    produceRay(0, 0, &origin00, &dest00);
    produceRay(0, RayTracingResolutionY - 1, &origin01, &dest01);
    produceRay(RayTracingResolutionX - 1, 0, &origin10, &dest10);
    produceRay(RayTracingResolutionX - 1, RayTracingResolutionY - 1, &origin11,
            &dest11);

    // Perform timing
    time_t start, end, ticks;
    start = clock();

    for(float y = 0;y < RayTracingResolutionY;++y){
        for(float x = 0;x < RayTracingResolutionX;++x){
            //svp, decidez vous memes quels parametres vous allez passer à la fonction
            //e.g., maillage, triangles, sphères etc.
            const float xscale = 1.0f - x / (RayTracingResolutionX - 1);
#ifdef WIN32
            const float yscale = float(y) / (RayTracingResolutionY - 1);
#else
            const float yscale = 1.0f - y / (RayTracingResolutionY - 1);
#endif
            origin = yscale * (xscale * origin00 + (1 - xscale) * origin10)
                    + (1 - yscale)
                            * (xscale * origin01 + (1 - xscale) * origin11);
            dest = yscale * (xscale * dest00 + (1 - xscale) * dest10)
                    + (1 - yscale) * (xscale * dest01 + (1 - xscale) * dest11);

            result.setPixel(x, y, performRayTracing(origin, dest));
        }
    }

    // calculate elapsed time
    end = clock();
    ticks = end - start;
    int millis = ticks * 1000 / CLOCKS_PER_SEC;

    printf("Rendering took %d ms\n", millis);

    // write to texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, RayTracingResolutionX,
            RayTracingResolutionY, 0, GL_RGB, GL_FLOAT, &result._image[0]);

    result.writeImage("result");
}

#define rayOrig ray.orig
#define rayDir ray.dir
#define VEWY_HIGH 10e6f

Vec3Df black(0, 0, 0);

//return the color of your pixel.
Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dest){
	Ray ray = Ray(black, origin, dest);

	// calculate nearest triangle
	int idx = -1; /* the triangle that was hit */
	float hit = VEWY_HIGH; /* distance to hit triangle */
	unsigned int numTriangles = MyMesh.triangles.size();
	for (unsigned int i = 0; i < numTriangles; i++){
		float ins = ray.intersect(MyMesh.triangles[i]);
		if (ins < VEWY_HIGH && ins < hit && ins > 0) {
			hit = ins;
			idx = i;
		}
	}


	// using black background
	if (idx == -1)
		return Vec3Df((rand() % 255) /255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f);

	Vec3Df& normal = MyMesh.triangles[idx].normal;
	float angle = -dot(normal, origin) * 0.5;

	return Vec3Df(angle, angle, angle);
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
