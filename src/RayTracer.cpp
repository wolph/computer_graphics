#include "RayTracer.hpp"
#include "Tree.hpp"
#include "Scene.hpp"
#include "Threading.hpp"

#include <ctime>
#include <algorithm>
#include <cmath>

using namespace std;

//temporary variables
extern unsigned int textures[2];
extern Scene MyScene;
int alternateX, alternateY;

ThreadPool pool(THREADS);
Image preview_image(PREVIEW_RES_X, PREVIEW_RES_Y);
Image output_image(RAYTRACE_RES_X, RAYTRACE_RES_Y);

float hardwood[720 * 720 * 3];

// runtime options
enum {SHADOW, CHECK, DEBUG, AMBIENT, DIFFUSE, SPECULAR, REFLECT, REFRACT, OCCLUSION, PHONG};
bool g_flags[10];
char* flagstrs[10] = {
	"shadow", "checkerboard", "debug", "ambient", "diffuse", "specular", "reflect", "refract",
	"occlusion", "phong"
};

bool threadsStarted = false;

Vec3Df origin00, dest00;
Vec3Df origin01, dest01;
Vec3Df origin10, dest10;
Vec3Df origin11, dest11;

queue<future<int>> asyncResults;
unsigned int asyncResultsSize;
bool asyncBuildStarted = false;
Timer asyncTimer;


//use this function for any preprocessing of the mesh.
int init(int argc, char **argv){
    srand(unsigned(time(0)));

    printf("Preview %dx%d@%d msaa using %d threads\n", PREVIEW_RES_X,
    PREVIEW_RES_Y, PREVIEW_MSAA, THREADS);
    printf("Raytrace %dx%d@%d msaa using %d threads\n", RAYTRACE_RES_X,
    RAYTRACE_RES_Y, MSAA, THREADS);
    printf("Window %dx%d\n", WINDOW_RES_X, WINDOW_RES_Y);

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
    for(int i = 0;i < 720 * 720;i++){
        hardwood[i * 3 + 0] = buf[i * 3 + 2] / 255.0f;
        hardwood[i * 3 + 1] = buf[i * 3 + 1] / 255.0f;
        hardwood[i * 3 + 2] = buf[i * 3 + 0] / 255.0f;
    }
    delete[] buf;
    fclose(fp);

    isRealtimeRaytracing = 1;
    isDrawingTexture = 0;

    return 0;
}

int threadedTracePart(Image* result, const unsigned int w, const unsigned int h,
        const unsigned int xa, const unsigned int ya, const unsigned int xb,
        const unsigned int yb) {
    Vec3Df origin, dest;
	vector<float>& image = result->data;

    const unsigned int msaa = isRealtimeRaytracing ? PREVIEW_MSAA : MSAA;

    for (float y = ya; y < yb; y++) {
        for (float x = xa; x < xb; x++) {
            Vec3Df total(0, 0, 0);
            float step = 1.0 / msaa;
            for(float xs = 0; xs < 1.0f; xs += step){
                for(float ys = 0; ys < 1.0f; ys += step){
                    float xscale = 1.0f - (x + xs) / (w - 1);
                    float yscale = float(y + ys) / (h - 1);
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
            total /= msaa;
            total /= msaa;

			// save it
            image[(y * w + x) * 3] = total.p[X];
            image[(y * w + x) * 3 + 1] = total.p[Y];
            image[(y * w + x) * 3 + 2] = total.p[Z];
        }
    }
    return yb;
}

void threadedTrace(Image* result, const unsigned int w, const unsigned int h, const bool background=true){
    Timer timer(10, 1);
    printf("preview part size: %d\n", PREVIEW_PART_SIZE);

    std::vector<std::pair<unsigned int, unsigned int>>parts;
    for(unsigned int i = 0;i < w;i +=
        PREVIEW_PART_SIZE){
        for(unsigned int j = 0;j < h;j +=
            PREVIEW_PART_SIZE){
            parts.push_back(pair<unsigned int, unsigned int>(i, j));
        }
    }

    while(isRealtimeRaytracing && pool.running() && background) {
        std::queue<std::future<int>> results;
        std::random_shuffle(parts.begin(), parts.end());
        for(pair<unsigned int, unsigned int> p: parts){
            results.push(
                         pool.enqueue(threadedTracePart, result, w, h,
                                      p.first, p.second,
                                      p.first+PREVIEW_PART_SIZE, p.second+PREVIEW_PART_SIZE));
        }


        while(!results.empty() && isRealtimeRaytracing){
            results.front().wait();
            results.pop();
        }
        if(timer.needsDisplay()){
            printf("Rendering took %.3f seconds\n", timer.avg());
            timer.updateLastDisplay();
        }
    }
    threadsStarted = false;
}

// make a 3D ray from screen positions
void createRay(int x_I, int y_I, Vec3Df* origin, Vec3Df* dest){
    int viewport[4];
	double projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projection); //recuperer matrices
    glGetIntegerv(GL_VIEWPORT, viewport); //viewport
    int y_new = viewport[3] - y_I;

    double x, y, z;
	double* modelview = MyScene.cam.viewmat;

    gluUnProject(x_I, y_new, 0, modelview, projection, viewport, &x, &y, &z);
    origin->p[X] = x;
    origin->p[Y] = y;
    origin->p[Z] = z;
    gluUnProject(x_I, y_new, 1, modelview, projection, viewport, &x, &y, &z);
    dest->p[X] = x;
    dest->p[Y] = y;
    dest->p[Z] = z;
}

void startRayTracing(int texIndex, bool needsRebuild) {
    // update scene
    Image& result = isRealtimeRaytracing ? preview_image : output_image;
    std::future_status status = std::future_status::deferred;

    unsigned int w = isRealtimeRaytracing ? PREVIEW_RES_X : RAYTRACE_RES_X;
    unsigned int h = isRealtimeRaytracing ? PREVIEW_RES_Y : RAYTRACE_RES_Y;
    w = alternateX ? alternateX : w;
    h = alternateY ? alternateY : h;

    if (needsRebuild)
        printf("Raytracing image with resolution of %d by %d\n", w, h);

    createRay(0, 0, &origin00, &dest00);
    createRay(0, WINDOW_RES_X - 1, &origin01, &dest01);
    createRay(WINDOW_RES_X - 1, 0, &origin10, &dest10);
    createRay(WINDOW_RES_X - 1, WINDOW_RES_Y - 1, &origin11, &dest11);

    if(!isRealtimeRaytracing){
        Timer timer(1, 0.2);

        if(needsRebuild){
            std::vector<std::pair<unsigned int, unsigned int>>parts;
            for(unsigned int i = 0;i < w;i +=
                PREVIEW_PART_SIZE){
                for(unsigned int j = 0;j < h;j +=
                    PREVIEW_PART_SIZE){
                    parts.push_back(pair<unsigned int, unsigned int>(i, j));
                }
            }

            random_shuffle(parts.begin(), parts.end());
            for(pair<unsigned int, unsigned int> p: parts){
                asyncResults.push(pool.enqueue(
                    threadedTracePart, &result, w, h,
                    p.first, p.second,
                    p.first+PREVIEW_PART_SIZE, p.second+PREVIEW_PART_SIZE));
            }

            asyncTimer.start();
            asyncBuildStarted = true;
            asyncResultsSize = (unsigned int)asyncResults.size();
        }

        while (!asyncResults.empty()){
            status = asyncResults.front().wait_for(chrono::milliseconds(25));
            if(status == std::future_status::ready){
                asyncResults.pop();
            } else if(status == future_status::timeout){
                break;
            }

            if (timer.needsDisplay()) {
                const unsigned int current = asyncResultsSize - (int)asyncResults.size();
                const unsigned int todo = (int)asyncResults.size();
                const double duration = asyncTimer.total() / 1000.;
                const double time_per_part = duration / current;
                printf("Rendered %d/%d in %.3f seconds: %.1f%%, eta: %.1f seconds\n",
                       current,
                       asyncResultsSize,
                       duration,
                       100. * current / asyncResultsSize,
                       time_per_part * todo
                );
                timer.updateLastDisplay();
            }
        }
    }else if(!threadsStarted){
        new std::thread(threadedTrace, &result, w, h, true);
        threadsStarted = true;
    }

    glTexImage2D(
                 GL_TEXTURE_2D,      // target
                 0,                  // level
                 GL_RGB,             // internalFormat
                 w,                  // width
                 h,                  // height
                 0,                  // border
                 GL_RGB,             // format
                 GL_FLOAT,           // type
                 &result.data[0]); // data

    if(isRealtimeRaytracing || asyncResults.empty()){
        if(!isRealtimeRaytracing && status == std::future_status::ready){
            result.write("result");
            printf("Rendered %d in %.1f seconds\n",
                asyncResultsSize,
                asyncTimer.next().count()
            );
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}

#define VEWY_HIGH 10e6f

inline Vec3Df background(Vec3Df orig, Vec3Df dir){

    float height = orig.p[Y] + 1,
		a = -height / dir.p[Y],
		x = orig.p[X] + a * dir.p[X],
		z = orig.p[Z] + a * dir.p[Z];

	Vec3Df tocam = Vec3Df(x, 0, z) - orig;

	float fogVar = -(tocam.getLength() * 0.05f + 1.0f);
	fogVar = (1.0f + fogVar ) / fogVar / 1.2f;
	Vec3Df fog = Vec3Df(1, 1, 1) * -fogVar; // Remove the minus for white fog

	if (dir.p[Y] < MyScene.floorheight){

		unsigned int shadows = (int)MyScene.lights.size();

		if (g_flags[SHADOW]){
        for(Vec3Df& light : MyScene.lights){
            Vec3Df impact = Vec3Df(x, MyScene.floorheight, z);
            Vec3Df tolight = light - impact;
            tolight.normalize();

            Vec3Df tempImpact, tempNormal;
            Material* tempMat;
            Object* tempObj;
			if (MyScene.raytrace(impact, tolight, &tempImpact, &tempNormal,
                    &tempMat, &tempObj))
				shadows--;
			}
        }
        float ratio = (float)shadows / (float)MyScene.lights.size();

        if(height < 0)
            return Vec3Df(0, 0.3f, 0) * ratio;

        if(g_flags[CHECK]){
            // checkerboard
            bool white = true;
            if(x > floor(x) + 0.5f)
                white = !white;
            if(z > floor(z) + 0.1f)
                white = !white;

            if(white)
                return Vec3Df(0.1f, 0.1f, 0.1f) * ratio;
            else
                return Vec3Df(0.9f, 0.9f, 0.9f) * ratio;
        }else{
            int xidx = (int)(x * 720 * 0.025) % 720;
            int zidx = (int)(z * 720 * 0.025) % 720;
            if(xidx < 0)
                xidx += 720;
            if(zidx < 0)
                zidx += 720;
			return *(Vec3Df*)&hardwood[(zidx * 720 + xidx) * 3] * ratio + fog;
        }
	} else {
		//if (!g_flags[CHECK])
        //    fog += Vec3Df(0, 0.6f, 0.99f);
        return fog;
	}
}

Vec3Df performRayTracing(const Vec3Df& orig, const Vec3Df& dir,
        const unsigned int depth, bool inside) {
    // calculate nearest triangle
    Object* obj;

    if(!depth)
        return Vec3Df(0, 0, 0);

    Vec3Df global_color;
    Vec3Df impact;
    Vec3Df normal;
    Material* mat2;
    MyScene.raytrace(orig, dir, &impact, &normal, &mat2, &obj, &global_color);
    Material& mat = *mat2;

    // background
    if(!obj || impact.p[1] < MyScene.floorheight) {
        return background(orig, dir);
    }

    Vec3Df tocam = orig - impact;
    tocam.normalize();

    // refraction
	if (g_flags[REFRACT]) {
		float idx = mat.refraction;
		if (!inside) idx = 1.0f / idx;
		float costh = dot(tocam, normal);
		float sinth = idx * idx * (1 - costh * costh);
		Vec3Df refr = idx * tocam + (idx * costh - sqrt(1 - sinth)) * normal;
		refr = -refr;

		global_color += performRayTracing(impact, refr, depth - 1, true);
    }

    Vec3Df lightColor(1, 1, 1);

    unsigned int shadows = (int)MyScene.lights.size();

    for (Vec3Df& light : MyScene.lights){
        Vec3Df color(global_color);
        Vec3Df tolight = light - impact;
        tolight.normalize();

        // ambient
        if(g_flags[AMBIENT] && mat.ambient){
            color += mat.Kd;
        }

        // shadow
		if (g_flags[SHADOW]){
        Vec3Df tempImpact, tempNormal;
        Material* tempMat;
        Object* tempObj;
        if(MyScene.raytrace(impact, tolight, &tempImpact, &tempNormal, &tempMat,
                &tempObj))
				shadows--;
		}

		// phong illumination
		// model source: http://www.cpp-home.com/tutorials/211_1.htm
		if (g_flags[PHONG]) {
			Vec3Df refl = 2 * normal * dot(normal, tolight) - tolight;
			Vec3Df res1 = mat.Kd * dot(normal, tolight);
			res1 += pow(mat.Ns * mat.Ks * dot(refl, dir), (float)(mat.Ns / mat.Ni));
			res1 *= lightColor * mat.Tr;
			if (g_flags[AMBIENT])
				res1 += mat.Ka * mat.Kd;
			color += res1;
		}

        // diffuse
        if (g_flags[DIFFUSE] && mat.color) {
            color += mat.Kd * lightColor * max(0.f, dot(normal, tolight));
        }

        // specular
        if(g_flags[SPECULAR] && mat.highlight){
            float LN = dot(tolight, normal);
            Vec3Df R = 2 * LN * normal - tolight;
            if(LN > 0){
                R.normalize();
                color += mat.Ks * pow(max(0.f, dot(tocam, R)), mat.Ns);
            }
        }

        // reflect
        if(g_flags[REFLECT] && mat.reflection){
            const Vec3Df r = dir - 2 * dot(dir, normal) * normal;
            color += performRayTracing(impact, r, depth - 1) * mat.Ni;
        }

        // occlusion
        global_color[0] = max(global_color[0], color[0]);
        global_color[1] = max(global_color[1], color[1]);
        global_color[2] = max(global_color[2], color[2]);
    }

    // shadow
    if (g_flags[SHADOW]) {
        global_color *= ((float)shadows / (float)MyScene.lights.size());
    }

    // return color
    for(int i = 0;i < 3;i++){
        if(global_color.p[i] > 1)
            global_color.p[i] = 1;
    }

    return global_color;
}

void drawCube(AABB* cube){
    if (cube->sub) {
        //glColor3f(1, 0.5, 0.5);
        for(int i = 0;i < 8;i++)
            drawCube(cube->sub[i]);
    }
    if (!cube->leaves.empty()) {
        //glColor3f(0, 0.5, 0.5);
        float dim = cube->radius * 2;
        for(int axis = 0;axis < 3;axis++){
            for(int x = 0;x < 2;x++){
                for(int y = 0;y < 2;y++){
                    Vec3Df v = cube->pos;
                    v.p[(axis + 1) % 3] += x * dim;
                    v.p[(axis + 2) % 3] += y * dim;
                    glVertex3f(v.p[X], v.p[Y], v.p[Z]);
                    glVertex3f(v.p[X] + ((axis == 0) ? dim : 0),
                            v.p[Y] + ((axis == 1) ? dim : 0),
                            v.p[Z] + ((axis == 2) ? dim : 0));
                }
            }
        }
    }
}

void yourDebugDraw(){
    // draw octree
    for(Object* obj : MyScene.objects){
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glTranslatef(obj->pos.p[0], obj->pos.p[1], obj->pos.p[2]);
        glBegin(GL_LINES);
        drawCube(obj->tree.root);
        glEnd();
        glPopAttrib();
    }
}

inline Vec3Df pow(Vec3Df in1, float in2){
	in1[X] = pow(in1[X], in2);
	in1[Y] = pow(in1[Y], in2);
	in1[Z] = pow(in1[Z], in2);
	return in1;
}
