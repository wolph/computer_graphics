#include "RayTracer.hpp"
#include "Tree.hpp"
#include <ctime>

//temporary variables
Vec3Df testRayOrigin;
Vec3Df testRayDestination;
string mesh;
extern unsigned int textures[2];
clock_t lastFrameTime;
float fps;
unsigned int framesSinceLastDraw = 30;
string screenFPS;
extern unsigned int previewResX;
extern unsigned int previewResY;
extern unsigned int msaa;
extern unsigned int numThreads;
extern Tree MyTree;

//use this function for any preprocessing of the mesh.
int init(int argc, char **argv){
    // skip program name argv[0] if present
    argc -= (argc > 0);
    argv += (argc > 0);

    option::Stats stats(usage, argc, argv);
    option::Option* options = (option::Option*)calloc(stats.options_max,
            sizeof(option::Option));
    option::Option* buffer = (option::Option*)calloc(stats.buffer_max,
            sizeof(option::Option));

    option::Parser parse(usage, argc, argv, options, buffer);

    if(parse.error())
        return 1;

    if(options[HELP]){
        int columns = getenv("COLUMNS") ? atoi(getenv("COLUMNS")) : 80;
        option::printUsage(fwrite, stdout, usage, columns);
        return 2;
    }

    lastFrameTime = clock();

    if(options[MESH]){
        const char* arg = options[MESH].last()->arg;
        if(arg != 0){
            mesh = arg;
        }
    }else{
        mesh = "0";
    }

    if(mesh == "0")
        mesh = "cube";
    else if(mesh == "1")
        mesh = "simple_monkey";
    else if(mesh == "2")
        mesh = "monkey";
    else if(mesh == "3")
        mesh = "dodgeColorTest";

    mesh = string("mesh/").append(mesh).append(".obj");
    MyMesh.loadMesh(mesh.c_str(), true);
    MyMesh.computeVertexNormals();
    MyTree.build(MyMesh);

    //one first move: initialize the first light source
    //at least ONE light source has to be in the scene!!!
    //here, we set it to the current location of the camera
    MyLightPositions.push_back(MyCameraPosition);

    if(options[RAYTRACE]){
        startRayTracing(activeTexIndex, true);
        return 255;
    }

    return 0;
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

Vec3Df origin00, dest00;
Vec3Df origin01, dest01;
Vec3Df origin10, dest10;
Vec3Df origin11, dest11;

void raytracePart(Image* result, int w, int h, int xx, int yy, int ww, int hh){
    Vec3Df origin, dest;
    for(float y = yy;y < hh;y++){
        for(float x = xx;x < ww;x++){
            //svp, decidez vous memes quels parametres vous allez passer à la fonction
            //c'est le stront a la plafond, c'est drôle
            //e.g., maillage, triangles, sphères etc.
            Vec3Df total(0, 0, 0);
            for(unsigned int xs = 0;xs < msaa;xs++){
                for(unsigned int ys = 0;ys < msaa;ys++){

                    float xscale = 1.0f - (x + float(xs) / msaa) / (w - 1);
                    float yscale = float(y + float(ys) / msaa) / (h - 1);
#ifdef LINUX
                    yscale = 1.0f - yscale;
#endif
                    origin = yscale
                            * (xscale * origin00 + (1 - xscale) * origin10)
                            + (1 - yscale)
                                    * (xscale * origin01
                                            + (1 - xscale) * origin11);
                    dest = yscale * (xscale * dest00 + (1 - xscale) * dest10)
                            + (1 - yscale)
                                    * (xscale * dest01 + (1 - xscale) * dest11);

                    total += performRayTracing(origin, dest);
                }
            }

            // calculate average color
            total /= msaa * msaa;
            // result->_image
            result->_image[(y * result->_width + x) * 3 + 0] = total[0];
            result->_image[(y * result->_width + x) * 3 + 1] = total[1];
            result->_image[(y * result->_width + x) * 3 + 2] = total[2];
        }
    }
}

void startRayTracing(int texIndex, bool verbose){
    if(verbose)
        cout << "Raytracing" << endl;
    int w = RayTracingResolutionX;
    int h = RayTracingResolutionY;
    if(!verbose)
        w = previewResX;
    if(!verbose)
        h = previewResY;
    Image result(w, h);

    produceRay(0, 0, &origin00, &dest00);
    produceRay(0, WindowSizeX - 1, &origin01, &dest01);
    produceRay(WindowSizeX - 1, 0, &origin10, &dest10);
    produceRay(WindowSizeX - 1, WindowSizeY - 1, &origin11, &dest11);

    // Perform timing
    time_t start, end, ticks;
    start = clock();

    // multithread
#if THREADS != 0
    std::thread** th = (std::thread**)alloca(numThreads * sizeof(std::thread*));
    int subw = w / numThreads;

    for(unsigned int i = 0;i < numThreads;i++)
    th[i] = new std::thread(raytracePart, &result, w, h, i * subw, 0,
            (i + 1) * subw, h);			// i * subw, 0, subw, h);

    // wait for them to finish
    for(unsigned int i = 0;i < numThreads;i++)
    th[i]->join();

    // kill them all
    for(unsigned int i = 0;i < numThreads;i++)
    delete th[i];
#else
    raytracePart(&result, w, h, 0, 0, w, h);
#endif

    // calculate elapsed time
    end = clock();
    ticks = end - start;
    start = end;
    int millis = ticks * 1000 / CLOCKS_PER_SEC;

    if(verbose)
        printf("Rendering took %d ms cpu seconds and %d ms wall time\n", millis,
                millis / max(numThreads, 1u));

    // write to texture
    glBindTexture(GL_TEXTURE_2D, textures[texIndex]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT,
            &result._image[0]);

    // calculate elapsed time
    end = clock();
    ticks = end - start;
    millis = ticks * 1000 / CLOCKS_PER_SEC;

    if(verbose)
        printf("Uploading to GPU took %d ms\n", millis);

    if(verbose)
        result.writeImage("result");
}

#define VEWY_HIGH 10e6f

Vec3Df black(0, 0, 0);

//return the color of your pixel.
Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dest){
    Ray ray = Ray(black, origin, dest);

	// calculate nearest triangle
	Triangle* triangle;
	float dist = MyTree.collide(ray, &triangle);

	Vec3Df light(17, 8, 20);
	Vec3Df lightColor(0.2, 0.3, 1.0);


	Vec3Df impact = origin + ray.dir * dist;
	Vec3Df tolight = light - impact;
	Vec3Df tocam = origin - impact;
	tolight.normalize();

	if (!triangle)
		return black;

	// start with black
	Vec3Df color = black;

	// ambient lighting
	color += Vec3Df(0.2, 0.2, 0.2);

	// diffuse
	float angle = dot(triangle->normal, tolight) * 2;
	if (angle > 0)
	color += angle * lightColor;

	// specular
	float angle2 = dot(tocam, tolight) * 0.5f;
	if (angle2 > 0)
	color += angle2 * lightColor;

	// reflection
	
	// return color
	return color;
}

void yourDebugDraw(){
    //draw open gl debug stuff
    //this function is called every frame

    drawFPS();

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

void drawFPS(){
    clock_t diff = clock() - lastFrameTime;
    lastFrameTime = clock();
    fps = 1 / ((float)diff / (float)CLOCKS_PER_SEC);

    if(framesSinceLastDraw++ > 29){
        framesSinceLastDraw = 0;
        screenFPS = to_string((int)fps);
    }

    glLoadIdentity();
    //glRasterPos2f(1.0f, 1.0f); // FPS draws on the lefthand bottom side of the screen now, if anyone knows how to get it to the lefthand top of the screen please fix it ;)
    for(unsigned int i = 0;i < screenFPS.length();i++){
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, screenFPS[i]);
    }
}
