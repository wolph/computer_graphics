#include "RayTracer.hpp"
#include "Tree.hpp"
#include "Scene.hpp"
#include <ctime>

#include <math.h>

//temporary variables
extern unsigned int textures[2];
extern Scene MyScene;
Object* monkey;
int alternateX, alternateY;

ThreadPool pool(THREADS);
Image preview_image(PREVIEW_RES_X, PREVIEW_RES_Y);
Image output_image(RAYTRACE_RES_X, RAYTRACE_RES_Y);

float hardwood[720 * 720 * 3];

// runtime options
bool g_phong = true;
bool g_checkerboard = false;
bool g_debug = false;

//use this function for any preprocessing of the mesh.
int init(int argc, char **argv){
    string mesh;
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
        int columns = getenv("COLUMNS") ? std::stoi(getenv("COLUMNS")) : 80;
        option::printUsage(fwrite, stdout, usage, columns);
        return 2;
    }

    if(options[SCENE]){
        // dostuff
    }

    if(options[RAYTRACE_X]){
        const char* arg = options[RAYTRACE_X].last()->arg;
        if(arg != 0){
            alternateX = std::stoi(arg);
        }
    }

    if(options[RAYTRACE_Y]){
        const char* arg = options[RAYTRACE_Y].last()->arg;
        if(arg != 0){
            alternateY = std::stoi(arg);
        }
    }

    MyScene.load("mesh/scene.txt");

    // load texture
    FILE* fp = fopen("mesh/hardwood.bmp", "rb");
    unsigned char* buf = new unsigned char[720 * 720 * 3];
    fseek(fp, 54, SEEK_SET);
    fread(buf, 1, 720 * 720 * 3, fp);
    for (int i = 0; i < 720 * 720; i++) {
        hardwood[i * 3 + 0] = buf[i * 3 + 2] / 255.0f;
        hardwood[i * 3 + 1] = buf[i * 3 + 1] / 255.0f;
        hardwood[i * 3 + 2] = buf[i * 3 + 0] / 255.0f;
    }
    delete[] buf;
    fclose(fp);

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

Vec3Df origin00, dest00;
Vec3Df origin01, dest01;
Vec3Df origin10, dest10;
Vec3Df origin11, dest11;

int raytracePart(Image* result, int w, int h, int xx, int yy, int ww, int hh){
    Vec3Df origin, dest;
    const unsigned int msaa = isRealtimeRaytracing ? PREVIEW_MSAA : MSAA;
    for(float y = yy;y < hh;y++){
        for(float x = xx;x < ww;x++){
            //svp, decidez vous memes quels parametres vous allez passer à la fonction
            //c'est le stront a la plafond, c'est drôle
            //e.g., maillage, triangles, sphères etc.
            Vec3Df total(0, 0, 0);
            float step = 1.0 / msaa;
            for(float xs = 0;xs < 1.0f;xs += step){
                for(float ys = 0;ys < 1.0f;ys += step){
                    float xscale = 1.0f - (x + xs) / (w - 1);
                    float yscale = float(y + ys) / (h - 1);
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
    return yy;
}

void startRayTracing(int texIndex, bool verbose){
    // update scene
    MyScene.update();

    Image& result = isRealtimeRaytracing ? preview_image : output_image;
    int w = isRealtimeRaytracing ? PREVIEW_RES_X : RAYTRACE_RES_X;
    int h = isRealtimeRaytracing ? PREVIEW_RES_Y : RAYTRACE_RES_Y;
    w = alternateX ? alternateX : w;
    h = alternateY ? alternateY : h;

    if(verbose)
        printf("Raytracing image with resolution of %d by %d\n", w, h);

    produceRay(0, 0, &origin00, &dest00);
    produceRay(0, WINDOW_RES_X - 1, &origin01, &dest01);
    produceRay(WINDOW_RES_X - 1, 0, &origin10, &dest10);
    produceRay(WINDOW_RES_X - 1, WINDOW_RES_Y - 1, &origin11, &dest11);

    // Perform timing
    time_t start, end, ticks;
    start = clock();

    // multithread
    std::queue<std::future<int>> results;
    for(int i = 0; i<w; i++){
        pool.enqueue(raytracePart, &result, w, h, i, 0, i + 1, h);
//        results.push(pool.enqueue(raytracePart, &result, w, h, i, 0, i + 1, h));
    }

//    while(!results.empty()){
//        results.front().wait();
//        results.pop();
//    }

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

// surface of triangle
// heron formula
float surface(float* t){

    // side lengths
    float a = sqrt(
            (t[0] - t[3]) * (t[0] - t[3]) + (t[1] - t[4]) * (t[1] - t[4])
                    + (t[2] - t[5]) * (t[2] - t[5]));
    float b = sqrt(
            (t[0] - t[6]) * (t[0] - t[6]) + (t[1] - t[7]) * (t[1] - t[7])
                    + (t[2] - t[8]) * (t[2] - t[8]));
    float c = sqrt(
            (t[3] - t[6]) * (t[3] - t[6]) + (t[4] - t[7]) * (t[4] - t[7])
                    + (t[5] - t[8]) * (t[5] - t[8]));

    // om
    float s = 0.5f * (a + b + c);

    // area
    float area = sqrt(s * (s - a) * (s - b) * (s - c));
    return area;
}

// incredibly beautiful convenience function
float surface(const Vec3Df& a, const Vec3Df& b, const Vec3Df& c){
    float f[9];
    *((Vec3Df*)&f[0]) = a;
    *((Vec3Df*)&f[3]) = b;
    *((Vec3Df*)&f[6]) = c;
    return surface(f);
}

inline Vec3Df background(Ray& ray){
    if(ray.dir.p[2] < 0){
        float height = ray.orig.p[2] + 1;
        float a = -height / ray.dir.p[2];
        float x = ray.orig.p[0] + a * ray.dir.p[0];
        float y = ray.orig.p[1] + a * ray.dir.p[1];
        if(height < 0)
            return Vec3Df(0, 0.3f, 0);

        if (g_checkerboard) {
            // checkerboard
            bool white = true;
            if (x > floor(x) + 0.5f)
                white = !white;
            if (y > floor(y) + 0.1f)
                white = !white;

            if (white)
                return Vec3Df(0.1f, 0.1f, 0.1f);
            else
                return Vec3Df(0.9f, 0.9f, 0.9f);
        }
        else {
            int xidx = (int)(x * 720 * 0.25) % 720;
            int yidx = (int)(y * 720 * 0.25) % 720;
            if (xidx < 0) xidx += 720;
            if (yidx < 0) yidx += 720;
            return *(Vec3Df*)&hardwood[(yidx * 720 + xidx) * 3];
        }
    } else
        return Vec3Df(0, 0.6f, 0.99f);
}

Vec3Df performRayTracing(const Vec3Df& orig, const Vec3Df& dir) {
	// calculate nearest triangle
	Triangle* triangle2;
	Object* obj;
	float dist = MyScene.raytrace(orig, dir, &triangle2, &obj);

	Vec3Df light(17, 8, 20);
	Vec3Df lightColor(0.2f, 0.3f, 1.0f);

	// background
	if (!triangle2)
		return background(Ray(orig, dir));

	const Vec3Df impact = orig + dir * dist;
	const Vec3Df tolight = normal(light - impact);
	const Vec3Df tocam = orig - impact;
	Vec3Df normal = triangle2->normal;

	if (g_phong) {
		// calc areas
		float a1 = surface(triangle2->vertices[1].p, impact + obj->pos,
			triangle2->vertices[2].p);
		float a2 = surface(triangle2->vertices[0].p, impact + obj->pos,
			triangle2->vertices[2].p);
		float a3 = surface(triangle2->vertices[0].p, impact + obj->pos,
			triangle2->vertices[1].p);
		float total = a1 + a2 + a3;

		// factors
		float f1 = a1 / total;
		float f2 = a2 / total;
		float f3 = a3 / total;

		// calc normal
		normal = f1 * triangle2->vertices[0].n + f2 * triangle2->vertices[1].n
			+ f3 * triangle2->vertices[2].n;
	}

	// ambient lighting
	Vec3Df color;

	// diffuse
	float angle = dot(normal, tolight) * 2;
	if (angle > 0)
		color += 0.3f * angle * lightColor;

	// specular
	float angle2 = dot(tocam, tolight) * 0.5f;
	if (angle2 > 0)
		color += 0.3f * angle2 * lightColor;

	//dont want infinite reflected rays
	//if(ray.bounceCount){
	// reflection
	const Vec3Df r = dir - 2 * dot(dir, normal)*normal;
	//Ray reflectedRay(ray.color, impact + r * 0.01f, impact + r, ray.bounceCount - 1);
	color += performRayTracing(impact, r) * 0.5f;

	// refraction
	/* Can't use this unless we switch away from .mtl files. Need density index for materials.
	float inIndex = 1;
	float outIndex = 1;
	float inDivOut = inIndex/outIndex;
	float cosIncident = dot(ray.dir, normal);
	float temp = inDivOut*inDivOut * 1-cosIncident*cosIncident;
	if(temp <= 1){
	Vec3Df t =inDivOut * ray.dir + (inDivOut * cosIncident - sqrt(1-temp))*normal;
	//Ray transmittedRay(ray.color, impact, impact + t, ray.bounceCount-1);
	} //temp > 1 means no refraction, only (total) reflection.
	*/
	// }

	for (unsigned int i = 0; i < MyLightPositions.size(); i++){
		Vec3Df lightPos = MyLightPositions[i];
		Vec3Df lightColor = performRayTracing(impact, lightPos);
		color += lightColor;
	}

	// return color
	for (int i = 0; i < 3; i++){
		if (color.p[i] > 1)
			color.p[i] = 1;
	}

	return color;
}

Vec3Df performRayTracing(Ray& ray){
    // calculate nearest triangle
    Triangle* triangle2;

    Object* obj;
    float dist = MyScene.raytrace(ray, &triangle2, &obj);

    Vec3Df light(17, 8, 20);
    Vec3Df lightColor(0.2f, 0.3f, 1.0f);

    // background
	if (!triangle2)
		return background(ray);

    const Vec3Df impact = ray.orig + ray.dir * dist;
    const Vec3Df tolight = normal(light - impact);
    const Vec3Df tocam = ray.orig - impact;
    Vec3Df normal = triangle2->normal;

    if (g_phong) {
        // calc areas
        float a1 = surface(triangle2->vertices[1].p, impact + obj->pos,
            triangle2->vertices[2].p);
        float a2 = surface(triangle2->vertices[0].p, impact + obj->pos,
            triangle2->vertices[2].p);
        float a3 = surface(triangle2->vertices[0].p, impact + obj->pos,
            triangle2->vertices[1].p);
        float total = a1 + a2 + a3;

        // factors
        float f1 = a1 / total;
        float f2 = a2 / total;
        float f3 = a3 / total;

        // calc normal
        normal = f1 * triangle2->vertices[0].n + f2 * triangle2->vertices[1].n
                + f3 * triangle2->vertices[2].n;
    }

    // ambient lighting
    Vec3Df color;

    // diffuse
    float angle = dot(normal, tolight) * 2;
    if(angle > 0)
        color += 0.3f * angle * lightColor;

    // specular
    float angle2 = dot(tocam, tolight) * 0.5f;
    if(angle2 > 0)
        color += 0.3f * angle2 * lightColor;

    //dont want infinite reflected rays
    //if(ray.bounceCount){
        // reflection
        Vec3Df r = ray.dir - 2 * dot(ray.dir, normal)*normal;
        Ray reflectedRay(ray.color, impact + r * 0.01f, impact + r, ray.bounceCount-1);
        color += performRayTracing(reflectedRay) * 0.5f;

        // refraction
        /* Can't use this unless we switch away from .mtl files. Need density index for materials.
        float inIndex = 1;
        float outIndex = 1;
        float inDivOut = inIndex/outIndex;
        float cosIncident = dot(ray.dir, normal);
        float temp = inDivOut*inDivOut * 1-cosIncident*cosIncident;
        if(temp <= 1){
            Vec3Df t =inDivOut * ray.dir + (inDivOut * cosIncident - sqrt(1-temp))*normal;
            //Ray transmittedRay(ray.color, impact, impact + t, ray.bounceCount-1);
        } //temp > 1 means no refraction, only (total) reflection.
        */
   // }

    for (unsigned int i = 0; i < MyLightPositions.size(); i++){
        Vec3Df lightPos = MyLightPositions[i];
        Ray r(impact, lightPos);
        Vec3Df lightColor = performRayTracing(r);
        color += lightColor;
    }

    // return color
    for(int i = 0;i < 3;i++){
        if(color.p[i] > 1)
            color.p[i] = 1;
    }

    return color;
}

void drawCube(AABB* cube){
    if(cube->sub){
        //glColor3f(1, 0.5, 0.5);
        for(int i = 0;i < 8;i++)
            drawCube(cube->sub[i]);
    }
    if(!cube->leaves.empty()){
        //glColor3f(0, 0.5, 0.5);
        float dim = cube->radius * 2;
        for(int axis = 0;axis < 3;axis++){
            for(int x = 0;x < 2;x++){
                for(int y = 0;y < 2;y++){
                    Vec3Df v = cube->pos;
                    v.p[(axis + 1) % 3] += x * dim;
                    v.p[(axis + 2) % 3] += y * dim;

                    glVertex3f(v.p[0], v.p[1], v.p[2]);
                    glVertex3f(v.p[0] + ((axis == 0) ? dim : 0),
                            v.p[1] + ((axis == 1) ? dim : 0),
                            v.p[2] + ((axis == 2) ? dim : 0));
                }
            }
        }
    }
}

void yourDebugDraw(){
    if (!isRealtimeRaytracing && !isDrawingTexture)
        MyScene.debugDraw();
}

void yourKeyboardPress(char t, int x, int y){
    if(t == 'w')
        monkey->vel.p[0] = 0.03f;
    if(t == 's')
        monkey->vel.p[0] = -0.03f;
    if(t == 'a')
        monkey->vel.p[1] = 0.03f;
    if(t == 'd')
        monkey->vel.p[1] = -0.03f;
}

void yourKeyboardRelease(char t, int x, int y){
    if(t == 'w')
        monkey->vel.p[0] = 0;
    if(t == 's')
        monkey->vel.p[0] = 0;
    if(t == 'a')
        monkey->vel.p[1] = 0;
    if(t == 'd')
        monkey->vel.p[1] = 0;
}
