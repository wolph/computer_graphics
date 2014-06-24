#include "RayTracer.hpp"
#include "Tree.hpp"
#include "Scene.hpp"
#include <ctime>
#include <algorithm>
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

bool g_ambient = true;
bool g_diffuse = true;
bool g_specular = true;
bool g_reflect = true;
bool g_refract = false;
bool g_occlusion = false;

bool threadsStarted = false;

Vec3Df origin00, dest00;
Vec3Df origin01, dest01;
Vec3Df origin10, dest10;
Vec3Df origin11, dest11;

//use this function for any preprocessing of the mesh.
int init(int argc, char **argv){
    std::srand(unsigned(std::time(0)));

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

    if(options[RAYTRACE]){
        startRayTracing(activeTexIndex, true);
        return 255;
    }

    return 0;
}

void periodicDraw(Image* image, unsigned int w, unsigned int h){
    std::thread([image, w, h](){
        while(isRealtimeRaytracing){
            // write to texture
            glBindTexture(GL_TEXTURE_2D, textures[0]);
            glTexImage2D(
                    GL_TEXTURE_2D,// target
                    0,// level
                    GL_RGB,// internalFormat
                    w,// width
                    h,// height
                    0,// border
                    GL_RGB,// format
                    GL_FLOAT,// type
                    &image->_image[0]);// data

            std::this_thread::sleep_for(std::chrono::milliseconds(1000/30));
        }
    }).detach();
}

int threadedTracePart(Image* result, const unsigned int w, const unsigned int h,
        const unsigned int xx, const unsigned int yy){
    Vec3Df origin, dest;
    std::vector<float>& image = result->_image;

    const unsigned int msaa = isRealtimeRaytracing ? PREVIEW_MSAA : MSAA;
    for(float y = yy;y < yy + PREVIEW_PART_SIZE;y++){
        for(float x = xx;x < xx + PREVIEW_PART_SIZE;x++){
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
            total /= msaa;
            total /= msaa;
            // result->_image
            image[(y * w + x) * 3] = total.p[0];
            image[(y * w + x) * 3 + 1] = total.p[1];
            image[(y * w + x) * 3 + 2] = total.p[2];
        }
    }
    return yy;
}

void threadedTrace(Image* result, const unsigned int w, const unsigned int h){
    // multithread
    while(isRealtimeRaytracing && pool.running()){
        std::queue<std::future<int>> results;
        for(unsigned int i = 0;i < w && isRealtimeRaytracing;i +=
        PREVIEW_PART_SIZE){
            for(unsigned int j = 0;j < h && isRealtimeRaytracing;j +=
            PREVIEW_PART_SIZE){
                results.push(
                        pool.enqueue(threadedTracePart, result, (unsigned int)w,
                                (unsigned int)h, (unsigned int)i,
                                (unsigned int)j));
            }
        }

        while(!results.empty() && isRealtimeRaytracing){
            results.front().wait();
            results.pop();
        }
    }
    threadsStarted = false;
}

//transformer le x, y en position 3D
void createRay(int x_I, int y_I, Vec3Df* origin, Vec3Df* dest){
    int viewport[4];
    double modelview[16];
    double projection[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview); //recuperer matrices
    glGetDoublev(GL_PROJECTION_MATRIX, projection); //recuperer matrices
    glGetIntegerv(GL_VIEWPORT, viewport); //viewport
    int y_new = viewport[3] - y_I;

    double x, y, z;

    gluUnProject(x_I, y_new, 0, modelview, projection, viewport, &x, &y, &z);
    origin->p[0] = x;
    origin->p[1] = y;
    origin->p[2] = z;
    gluUnProject(x_I, y_new, 1, modelview, projection, viewport, &x, &y, &z);
    dest->p[0] = x;
    dest->p[1] = y;
    dest->p[2] = z;
}

void startRayTracing(int texIndex, bool verbose){
    // update scene
    Image& result = isRealtimeRaytracing ? preview_image : output_image;

    int w = isRealtimeRaytracing ? PREVIEW_RES_X : RAYTRACE_RES_X;
    int h = isRealtimeRaytracing ? PREVIEW_RES_Y : RAYTRACE_RES_Y;
    w = alternateX ? alternateX : w;
    h = alternateY ? alternateY : h;

    if(verbose)
        printf("Raytracing image with resolution of %d by %d\n", w, h);

    createRay(0, 0, &origin00, &dest00);
    createRay(0, WINDOW_RES_X - 1, &origin01, &dest01);
    createRay(WINDOW_RES_X - 1, 0, &origin10, &dest10);
    createRay(WINDOW_RES_X - 1, WINDOW_RES_Y - 1, &origin11, &dest11);

    if(!threadsStarted){
        new std::thread(threadedTrace, &result, w, h);
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
            &result._image[0]); // data
    std::this_thread::sleep_for(std::chrono::milliseconds(25));

    // Perform timing
    Timer timer(1);

    if(verbose)
        // calculate elapsed time
        printf("Rendering took %.3f seconds\n", timer.next().count());

    if(verbose)
        result.writeImage("result");
}

#define VEWY_HIGH 10e6f

inline Vec3Df background(Vec3Df orig, Vec3Df dir){

    if(dir.p[Y] < -6){
        float height = orig.p[Y] + 1;
        float a = -height / dir.p[Y];
        float x = orig.p[X] + a * dir.p[X];
        float z = orig.p[Z] + a * dir.p[Z];

        unsigned int shadows = 0;
        for(Vec3Df& light : MyScene.lights){
            Vec3Df impact = Vec3Df(x, -6, z);
            Vec3Df tolight = light - impact;
            tolight.normalize();

            Vec3Df tempImpact, tempNormal;
            Material* tempMat;
            Object* tempObj;
            if(!MyScene.raytrace(impact, tolight, &tempImpact, &tempNormal,
                    &tempMat, &tempObj))
                shadows++;
        }
        float ratio = (float)shadows / (float)MyScene.lights.size();

        if(height < 0)
            return Vec3Df(0, 0.3f, 0) * ratio;

        if(g_checkerboard){
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
            int xidx = (int)(x * 720 * 0.25) % 720;
            int zidx = (int)(z * 720 * 0.25) % 720;
            if(xidx < 0)
                xidx += 720;
            if(zidx < 0)
                zidx += 720;
            return *(Vec3Df*)&hardwood[(zidx * 720 + xidx) * 3] * ratio;
        }
    }else
        return Vec3Df(0, 0.6f, 0.99f);
}

Vec3Df performRayTracing(const Vec3Df& orig, const Vec3Df& dir,
        const unsigned int depth){
    // calculate nearest triangle
    Object* obj;
    Vec3Df color;

    if(!depth)
        return Vec3Df(0, 0, 0);

    Vec3Df impact;
    Vec3Df normal;
    Material* mat2;
    MyScene.raytrace(orig, dir, &impact, &normal, &mat2, &obj);
    Material& mat = *mat2;

    // background
    if(!obj){
        return background(orig, dir);
    }

    Vec3Df tocam = orig - impact;
    tocam.normalize();

    // refraction
    /* Can't use this unless we switch away from .mtl files. Need density
     index for materials. */
    if(g_refract){
        float inIndex = 1;
        float outIndex = 1;
        float inDivOut = inIndex / outIndex;
        float cosIncident = dot(dir, normal);
        float temp = inDivOut * inDivOut * 1 - cosIncident * cosIncident;
        if(temp <= 1){
            Vec3Df t = inDivOut * dir
                    + (inDivOut * cosIncident - sqrt(1 - temp)) * normal;
            color += performRayTracing(impact, t, depth - 1);
        } //temp > 1 means no refraction, only (total) reflection.
    }
    Vec3Df lightColor(1, 1, 1);

    unsigned int shadows = 0;

    for(Vec3Df& light : MyScene.lights){
        Vec3Df tolight = light - impact;
        tolight.normalize();

        // ambient
        if(g_ambient && mat.ambient)
            color += mat.Kd * 0.1f;

        // shadow
        Vec3Df tempImpact, tempNormal;
        Material* tempMat;
        Object* tempObj;
        if(MyScene.raytrace(impact, tolight, &tempImpact, &tempNormal, &tempMat,
                &tempObj))
            shadows++;

        // diffuse
        if(g_diffuse && mat.color){
            float angle = dot(normal, tolight) * 2;
            color += lightColor * angle * mat.Kd;
        }

        // specular
        if(g_specular && mat.highlight){
            Vec3Df half = (tocam + tolight) * 0.5f;
            float spec = pow(dot(half, normal), 1.5f);
            color += lightColor * spec * mat.Ks * 0.5f;
        }

        // reflect
        if(g_reflect && mat.reflection){
            const Vec3Df r = dir - 2 * dot(dir, normal)*normal;
            color += performRayTracing(impact, r, depth - 1) * 0.25f;
        }

        // refract
        // occlusion

        // shadow
    }
    color *= ((float)shadows / (float)MyScene.lights.size());

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
    if(!isRealtimeRaytracing && !isDrawingTexture)
        MyScene.debugDraw();
}

#define MOVE_VELOCITY 0.05f

bool yourKeyboardPress(char key, int x, int y){
    switch(key){
        /* Toggles */
        case '1':
            g_phong = !g_phong;
            printf("Set phong to %d\n", g_phong);
            break;
        case '2':
            g_checkerboard = !g_checkerboard;
            printf("Set checkerboard to %d\n", g_checkerboard);
            break;
        case '3':
            g_debug = !g_debug;
            printf("Set debug to %d\n", g_debug);
            break;
        case '4':
            g_ambient = !g_ambient;
            printf("Set ambient to %d\n", g_ambient);
            break;
        case '5':
            g_diffuse = !g_diffuse;
            printf("Set diffuse to %d\n", g_diffuse);
            break;
        case '6':
            g_specular = !g_specular;
            printf("Set specular to %d\n", g_specular);
            break;
        case '7':
            g_reflect = !g_reflect;
            printf("Set reflect to %d\n", g_reflect);
            break;
        case '8':
            g_refract = !g_refract;
            printf("Set refract to %d\n", g_refract);
            break;
        case '9':
            g_occlusion = !g_occlusion;
            printf("Set occlusion to %d\n", g_occlusion);
            break;

            /* Movement */
        case 'a':
            MyScene.object->vel.p[X] = -MOVE_VELOCITY;
            break;
        case 'd':
            MyScene.object->vel.p[X] = MOVE_VELOCITY;
            break;
        case 'q':
            MyScene.object->vel.p[Z] = -MOVE_VELOCITY;
            break;
        case 'e':
            MyScene.object->vel.p[Z] = MOVE_VELOCITY;
            break;
        case 'w':
            MyScene.object->vel.p[Y] = MOVE_VELOCITY;
            break;
        case 's':
            MyScene.object->vel.p[Y] = -MOVE_VELOCITY;
            break;

            /* Object selection */
        case '+':
        case '=':
            MyScene.nextObject();
            break;
        case '-':
        case '_':
            MyScene.prevObject();
            break;
        default:
            return false;
            break;
    }
    return true;
}

bool yourKeyboardRelease(char t, int x, int y){
    switch(t){
        case 'a':
        case 'd':
            MyScene.object->vel.p[X] = 0;
            break;
        case 'q':
        case 'e':
            MyScene.object->vel.p[Z] = 0;
            break;
        case 'w':
        case 's':
            MyScene.object->vel.p[Y] = 0;
            break;
        default:
            return false;
            break;
    }
    return true;
}
