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

ThreadPool pool(getNumberOfCores());
Image preview_image(PREVIEW_RES_X, PREVIEW_RES_Y);
Image output_image(RAYTRACE_RES_X, RAYTRACE_RES_Y);

// runtime options
enum {SHADOW, CHECK, DEBUG, AMBIENT, DIFFUSE, SPECULAR, REFLECT, REFRACT, OCCLUSION, PHONG};
bool g_flags[10];
char* flagstrs[10] = {
	"shadow", "checkerboard", "debug", "ambient", "diffuse", "specular", "reflect", "refract",
	"occlusion", "phong"
};

// rays
//SRAY ray00, ray01, ray10, ray11;
Vec3Df origin00, dest00;
Vec3Df origin01, dest01;
Vec3Df origin10, dest10;
Vec3Df origin11, dest11;

//use this function for any preprocessing of the mesh.
int init(int argc, char **argv) {
    srand(time(0));

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

    if (parse.error())
        return 1;

    if (options[HELP]) {
        int columns = getenv("COLUMNS") ? stoi(getenv("COLUMNS")) : 80;
        option::printUsage(fwrite, stdout, usage, columns);
        return 2;
    }

    if (options[SCENE]) {
        // dostuff
    }

    if (options[RAYTRACE_X]) {
        const char* arg = options[RAYTRACE_X].last()->arg;
        if(arg != 0){
            //alternateX = stoi(arg);
        }
    }

    if (options[RAYTRACE_Y]) {
        const char* arg = options[RAYTRACE_Y].last()->arg;
        if(arg != 0){
            //alternateY = stoi(arg);
        }
    }

    MyScene.load("mesh/scene.txt");

    // load texture
    

    isRealtimeRaytracing = 1;
    isDrawingTexture = 0;

    return 0;
}


// make a 3D ray from screen positions
void createRay(int x_I, int y_I, Vec3Df* origin, Vec3Df* dest) {
	int viewport[4];
	float projection[16];
	glGetFloatv(GL_PROJECTION_MATRIX, projection); //recuperer matrices
	glGetIntegerv(GL_VIEWPORT, viewport); //viewport
	int y_new = viewport[3] - y_I;

	double x, y, z;
	float* modelview = MyScene.cam.viewmat;

	double modelview2[16];
	double projection2[16];
	for (int i = 0; i < 16; i++) {
		modelview2[i] = (double)modelview[i];
		projection2[i] = (double)projection[i];
	}

	gluUnProject(x_I, y_new, 0, modelview2, projection2, viewport, &x, &y, &z);
	//SETVEC((ray + 0), x, y, z);
	origin->x = (float)x;
	origin->y = (float)y;
	origin->z = (float)z;
	gluUnProject(x_I, y_new, 1, modelview2, projection2, viewport, &x, &y, &z);
	dest->x = (float)x;
	dest->y = (float)y;
	dest->z = (float)z;
}

void singleBgTrace(RAY ray, OUT COLOR color) {

	float height = ray[1] + MyScene.floorheight;
	float a = -height / ray[4];
	float x = ray[0] + a * ray[3];
	float z = ray[2] + a * ray[5];

	//Vec3Df tocam = Vec3Df(x, MyScene.floorheight, z) - orig;

	//float fogVar = -(tocam.getLength() * 0.05f + 1.0f);
	//fogVar = (1.0f + fogVar) / fogVar / 1.2f;
	//Vec3Df fog = Vec3Df(1, 1, 1) * fogVar; // Remove the minus for white fog

	if (1){//dir.p[Y] < MyScene.floorheight){

		unsigned int shadows = (int)MyScene.lights.size();

		/*if (g_flags[SHADOW]){
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
		float ratio = (float)shadows / (float)MyScene.lights.size();*/

		//if(height < 0)
		//   return Vec3Df(0, 0.3f, 0) * ratio;

		if (true){// g_flags[CHECK]){
			// checkerboard
			bool white = true;
			if (x > floor(x) + 0.5f)
				white = !white;
			if (z > floor(z) + 0.5f)
				white = !white;

			if (white) {
				SETCOLOR(color, 0.1, 0.1, 0.1);
				//return Vec3Df(0.1f, 0.1f, 0.1f) * ratio;
			}
			else {
				SETCOLOR(color, 0.9, 0.9, 0.9);
			}
			return;
		}
		else {
			int xidx = (int)(x * 720 * 0.06125) % 720;
			int zidx = (int)(z * 720 * 0.06125) % 720;
			if (xidx < 0)
				xidx += 720;
			if (zidx < 0)
				zidx += 720;
			//return *(Vec3Df*)&hardwood[(zidx * 720 + xidx) * 3] * ratio + fog;
		}
	}
	else {
		//if (!g_flags[CHECK])
		//    fog += Vec3Df(0, 0.6f, 0.99f);
		//return fog;
	}
}

// the raytracing of a single ray
void singleTrace(COLOR color, RAY ray, float weight, bool inside) {
	// calculate nearest triangle

	if (weight < 0.01f) {
		SETCOLOR(color, 0.5, 0.5, 0.5);
		return;
	}

	SVEC impact, normal;

	SVEC ambient, diffuse;
	SETCOLOR(ambient, 0.5, 0.5, 0.5);
	SETCOLOR(diffuse, 0.2, 0.4, 0.5);

	bool bg = MyScene.raytrace(ray, impact, normal);

	// background
	if (bg) {
		singleBgTrace(ray, color);
		return;
	}

	// refraction
	/*if (g_flags[REFRACT]) {
	float idx = mat.refraction;
	if (!inside) idx = 1.0f / idx;
	float costh = dot(tocam, normal);
	float sinth = idx * idx * (1 - costh * costh);
	Vec3Df refr = idx * tocam + (idx * costh - sqrt(1 - sinth)) * normal;
	refr = -refr;

	global_color += performRayTracing(impact, refr, depth - 1, true);
	}*/

	Vec3Df lightColor(1, 1, 1);

	unsigned int shadows = (int)MyScene.lights.size();

	for (Vec3Df& light : MyScene.lights) {
		SVEC color, tolight;
		//Vec3Df color(global_color);
		//Vec3Df tolight = light - impact;
		//tolight.normalize();

		//SUB(tolight, light, impact);

		// ambient
		if (g_flags[AMBIENT]) {
			ADD(color, ambient);
		}

		// shadow
		/*if (g_flags[SHADOW]){
		Vec3Df tempImpact, tempNormal;
		Material* tempMat;
		Object* tempObj;
		if(MyScene.raytrace(impact, tolight, &tempImpact, &tempNormal, &tempMat,
		&tempObj))
		shadows--;
		}*/

		// phong illumination
		// model source: http://www.cpp-home.com/tutorials/211_1.htm
		/*if (g_flags[PHONG]) {
		Vec3Df refl = 2 * normal * dot(normal, tolight) - tolight;
		Vec3Df res1 = mat.Kd * dot(normal, tolight);
		res1 += pow(mat.Ns * mat.Ks * dot(refl, dir), (float)(mat.Ns / mat.Ni));
		res1 *= lightColor * mat.Tr;
		if (g_flags[AMBIENT])
		res1 += mat.Ka * mat.Kd;
		color += res1;
		}*/

		// diffuse
		if (g_flags[DIFFUSE]) {
			SVEC diffuse2;
			float dot2;

			DOT(dot2, normal, tolight);
			MULS(diffuse, dot2);

			ADD(color, diffuse);
		}

		// specular
		/*if(g_flags[SPECULAR] && mat.highlight){
		float LN = dot(tolight, normal);
		Vec3Df R = 2 * LN * normal - tolight;
		if(LN > 0){
		R.normalize();
		color += mat.Ks * pow(max(0.f, dot(tocam, R)), mat.Ns);
		}
		}*/

		// reflect
		/*if (g_flags[REFLECT]){// && mat.reflection){
		const Vec3Df r = dir - 2 * dot(dir, normal) * normal;
		color += performRayTracing(impact, r, depth - 1);// *mat.Ni;
		}

		// occlusion
		global_color[0] = max(global_color[0], color[0]);
		global_color[1] = max(global_color[1], color[1]);
		global_color[2] = max(global_color[2], color[2]);*/
	}

	// shadow
	//if (g_flags[SHADOW]) {
	//    global_color *= ((float)shadows / (float)MyScene.lights.size());
	//}

	// return color
	//for(int i = 0;i < 3;i++){
	//    if(global_color.p[i] > 1)
	//        global_color.p[i] = 1;
	//}
}

// render a small part of the image
int renderPart(Image* result, int w, int h, int xa, int ya, int xb, int yb) {
	Vec3Df origin, dest;
	vector<float>& image = result->data;

	const unsigned int msaa = isRealtimeRaytracing ? PREVIEW_MSAA : MSAA;

	for (float y = ya; y < yb; y++) {
		for (float x = xa; x < xb; x++) {
			SCOLOR total;
			SETCOLOR(total, 0, 0, 0);

			float step = 1.0 / msaa;
			for (float xs = 0; xs < 1.0f; xs += step){
				for (float ys = 0; ys < 1.0f; ys += step){
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
					dest.normalize();

					SRAY ray;
					COPY3(ray + 0, origin.p);
					COPY3(ray + 3, dest.p);

					SVEC color;

					singleTrace(color, ray, 1.0f, false);
					ADD(total, color);
				}
			}

			// calculate average color
			//total /= msaa;
			//total /= msaa;
			DIV(total, msaa * msaa);

			// save it
			image[(y * w + x) * 3] = total[0];
			image[(y * w + x) * 3 + 1] = total[1];
			image[(y * w + x) * 3 + 2] = total[2];
		}
	}
	return yb;
}

// raytrace multithreaded
void renderChunks(Image* result, int w, int h) {
	// create the parts
    vector<pair<int, int>> parts;
    for(unsigned i = 0; i < w; i += PREVIEW_PART_SIZE)
        for(unsigned j = 0; j < h; j += PREVIEW_PART_SIZE)
            parts.push_back(pair<int, int>(i, j));
	random_shuffle(parts.begin(), parts.end());

	// enqueue them in the thread pool
	queue<future<int>> results;
	for (pair<int, int> p : parts){
		results.push(
			pool.enqueue(renderPart, result, w, h,
			p.first, p.second,
			p.first + PREVIEW_PART_SIZE, p.second + PREVIEW_PART_SIZE));
	}

	// wait for all parts to finish
	while (!results.empty() && isRealtimeRaytracing) {
		results.front().wait();
        results.pop();
    }
}

// main raytrace function
void render(Image& image) {
	// update scene
	Image& result = image;
	future_status status = future_status::deferred;

	int w = PREVIEW_RES_X;
	int h = PREVIEW_RES_Y;

	// create corner rays
	createRay(0, 0, &origin00, &dest00);
	createRay(0, WINDOW_RES_X - 1, &origin01, &dest01);
	createRay(WINDOW_RES_X - 1, 0, &origin10, &dest10);
	createRay(WINDOW_RES_X - 1, WINDOW_RES_Y - 1, &origin11, &dest11);

	renderChunks(&result, w, h);

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
}
