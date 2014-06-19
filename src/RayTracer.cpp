#include "RayTracer.hpp"
#include "Tree.hpp"
#include <ctime>
#include <math.h>

//temporary variables
Vec3Df testRayOrigin;
Vec3Df testRayDestination;
string mesh;
extern unsigned int textures[2];
extern Tree MyTree;
int alternateX, alternateY;

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

	mesh = "0";
    if(options[MESH]){
        const char* arg = options[MESH].last()->arg;
        if(arg != 0){
            mesh = arg;
        }
    }

	if (options[RAYTRACEX]){
		const char* arg = options[RAYTRACEX].last()->arg;
		if (arg != 0){
			alternateX = stoi(arg);
		}
	}

	if(options[RAYTRACEY]){
		const char* arg = options[RAYTRACEY].last()->arg;
		if (arg != 0){
			alternateY = stoi(arg);
		}
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
            for(unsigned int xs = 0;xs < MSAA;xs++){
                for(unsigned int ys = 0;ys < MSAA;ys++){
                    float xscale = 1.0f - (x + float(xs) / MSAA) / (w - 1);
                    float yscale = float(y + float(ys) / MSAA) / (h - 1);
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
            total /= MSAA * MSAA;
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
    int w = verbose ? RAYTRACE_RES_X : PREVIEW_RES_X;
    int h = verbose ? RAYTRACE_RES_Y : PREVIEW_RES_Y;
	w = alternateX ? alternateX : w;
	h = alternateY ? alternateY : h;

    Image result(w, h);

    produceRay(0, 0, &origin00, &dest00);
    produceRay(0, WINDOW_RES_X - 1, &origin01, &dest01);
    produceRay(WINDOW_RES_X - 1, 0, &origin10, &dest10);
    produceRay(WINDOW_RES_X - 1, WINDOW_RES_Y - 1, &origin11, &dest11);

    // Perform timing
    time_t start, end, ticks;
    start = clock();

    // multithread
#if THREADS != 0
    std::thread** th = (std::thread**)alloca(THREADS * sizeof(std::thread*));
    int subw = w / THREADS;

    for(unsigned int i = 0;i < THREADS;i++)
    th[i] = new std::thread(raytracePart, &result, w, h, i * subw, 0,
            (i + 1) * subw, h);			// i * subw, 0, subw, h);

    // wait for them to finish
    for(unsigned int i = 0;i < THREADS;i++)
    th[i]->join();

    // kill them all
    for(unsigned int i = 0;i < THREADS;i++)
    delete th[i];
#else
    raytracePart(&result, w, h, 0, 0, w, h);
#endif

    // calculate elapsed time
    end = clock();
    ticks = end - start;
    start = end;
    float millis = ticks * 1000. / CLOCKS_PER_SEC;

    if(verbose)
        printf("Rendering took %.0f ms cpu seconds and %.0f ms wall time\n",
                millis, millis / fmax(THREADS, 1));

    // write to texture
    glBindTexture(GL_TEXTURE_2D, textures[texIndex]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT,
            &result._image[0]);

    // calculate elapsed time
    end = clock();
    ticks = end - start;
    millis = ticks * 1000 / CLOCKS_PER_SEC;

    if(verbose)
        printf("Uploading to GPU took %.0f ms\n", millis);

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
    Vec3Df lightColor(0.2f, 0.3f, 1.0f);

    Vec3Df impact = origin + ray.dir * dist;
    Vec3Df tolight = light - impact;
    Vec3Df tocam = origin - impact;
    tolight.normalize();

    // background
    if(!triangle){
        if(ray.dir.p[2] < 0){
			float height = origin.p[2];
            float a = -height / ray.dir.p[2];
            float x = origin.p[0] + a * ray.dir.p[0];
			float y = origin.p[1] + a * ray.dir.p[1];
			if (height < 0)
				return Vec3Df(0, 0.3f, 0);

            bool white = true;
			if (x > floor(x) + 0.5f) white = !white;
			if (y > floor(y) + 0.5f) white = !white;

            if(white)
                return Vec3Df(0.1f, 0.1f, 0.1f);
            else
                return Vec3Df(0.9f, 0.9f, 0.9f);
        }else
            return Vec3Df(0, 0.6, 0.99);
    }

    // ambient lighting
    Vec3Df color;
    color += Vec3Df(0.2f, 0.2f, 0.2f);

    // diffuse
    float angle = dot(triangle->normal, tolight) * 2;
    if(angle > 0)
        color += angle * lightColor;

    // specular
    float angle2 = dot(tocam, tolight) * 0.5f;
    if(angle2 > 0)
        color += angle2 * lightColor;

    // reflection
    Vec3Df r = ray.dir - 2*dot(ray.dir, triangle->normal)*triangle->normal;
    Ray reflectedRay = Ray(ray.color, impact, impact + r);


    // refraction
    float inIndex = 1;
    float outIndex = 1;
    float inDivOut = inIndex/outIndex;
    float cosIncident = dot(ray.dir, triangle->normal);
    float temp = inDivOut*inDivOut * 1-cosIncident*cosIncident;
    if(temp <= 1) {
    	float temp2 = inDivOut *  cosIncident - sqrt(1-temp);
    	Vec3Df t =inDivOut * ray.dir + Vec3Df(temp2, temp2, temp);
    	Ray transmittedRay = Ray(ray.color, impact, impact + t);
    } //temp > 1 means no refraction, only (total) reflection.

    // return color
    return color;
}

void drawCube(AABB* cube) {
	if (cube->sub) {
		//glColor3f(1, 0.5, 0.5);
		for (int i = 0; i < 8; i++)
			drawCube(cube->sub[i]);
	}
	if (!cube->leaves.empty()) {
		//glColor3f(0, 0.5, 0.5);
		float dim = cube->radius * 2;
		for (int axis = 0; axis < 3; axis++) {
			for (int x = 0; x < 2; x++) {
				for (int y = 0; y < 2; y++) {
					Vec3Df v = cube->pos;
					v.p[(axis + 1) % 3] += x * dim;
					v.p[(axis + 2) % 3] += y * dim;

					glVertex3f(v.p[0], v.p[1], v.p[2]);
					glVertex3f(
						v.p[0] + ((axis == 0) ? dim : 0),
						v.p[1] + ((axis == 1) ? dim : 0),
						v.p[2] + ((axis == 2) ? dim : 0)
						);
				}
			}
		}
	}
}

void yourDebugDraw(){
	glColor3f(1, 0.5, 0.5);
	glLineWidth(10);
	glBegin(GL_LINES);
	drawCube(MyTree.root);
	glEnd();
}

void yourKeyboardFunc(char t, int x, int y){
    // do what you want with the keyboard input t.
    // x, y are the screen position

    //here I use it to get the coordinates of a ray, which I then draw in the debug function.
    produceRay(x, y, testRayOrigin, testRayDestination);

    std::cout << t << " pressed! The mouse was in location " << x << "," << y
            << "!" << std::endl;
}
