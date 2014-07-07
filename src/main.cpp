#include <Windows.h>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <GL/glut.h>
#include <assert.h>
#include <cstdlib>

#include "Vec3D.hpp"
#include "Timer.hpp"
#include "constants.hpp"
#include "mouse.hpp"
#include "RayTracer.hpp"
#include "Scene.hpp"

// runtime options

enum { SHADOW, CHECK, DEBUG, AMBIENT, DIFFUSE, SPECULAR, REFLECT, REFRACT, OCCLUSION, PHONG };
extern bool g_flags[10];
extern char* flagstrs[10];

// double buffered
unsigned int textures[2];
unsigned int activeTexIndex = 0;
unsigned int isDrawingTexture = 0;
unsigned int isRealtimeRaytracing = 0;
Scene MyScene;

bool needRebuild = false; // if the raytrace needs to be built


void specialKeyboardUp(int key, int x, int y){
	switch (key){
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN:
		MyScene.cam.rotateY = 0;
		break;
	case GLUT_KEY_RIGHT:
	case GLUT_KEY_LEFT:
		MyScene.cam.rotateX = 0;
		break;
	default:
		printf("Unknown special key %d\n", key);
		break;
	}
}

void specialKeyboard(int key, int x, int y){
	switch (key){
	case GLUT_KEY_UP:
		MyScene.cam.rotateY = 1;
		break;
	case GLUT_KEY_DOWN:
		MyScene.cam.rotateY = -1;
		break;
	case GLUT_KEY_RIGHT:
		MyScene.cam.rotateX = 1;
		break;
	case GLUT_KEY_LEFT:
		MyScene.cam.rotateX = -1;
		break;
	default:
		printf("Unknown special key %d\n", key);
		break;
	}
}

// when the window is reshaped
void reshape(int w, int h){
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, (float)w / h, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

// when a key is pressed

#define MOVE_VELOCITY 0.5f
void keyboard(unsigned char key, int x, int y){
	switch (key){
	case 'L':
		MyScene.addLightPoint(MyScene.cam.pos);
		break;
	case 'l':
		MyScene.lights[MyScene.lights.size() - 1] = MyScene.cam.pos;
		break;
	case 'r':
		needRebuild = true;
		isDrawingTexture = 1;
		isRealtimeRaytracing = 0;
		break;
	case 'b':
		if (isRealtimeRaytracing){
			isDrawingTexture = 0;
			isRealtimeRaytracing = 0;
		}
		else {
			cout << "Using " << getNumberOfCores() << " threads and resolution of "
				<< PREVIEW_RES_X << "x" << PREVIEW_RES_Y << endl;
			isRealtimeRaytracing = 1;
			isDrawingTexture = 0;
		}
		break;

		/* Escape */
	case 27:
		isRealtimeRaytracing = false;
		// TODO: fix this horrible hack to stop segfaults when exiting
		this_thread::sleep_for(chrono::milliseconds(1000));
		exit(0);

		/* Movement */
	case 'a': MyScene.cam.side = 1;	break;
	case 'd': MyScene.cam.side = -1; break;
	case 'q': MyScene.cam.alt = 1; break;
	case 'e': MyScene.cam.alt = -1; break;
	case 'w': MyScene.cam.forward = 1; break;
	case 's': MyScene.cam.forward = -1; break;

	default:
		// flags
		if ('0' <= key && key <= '9') {
			int flag = key - '0';
			g_flags[flag] = !g_flags[flag];
			printf("%s := %s\n", flagstrs[flag], g_flags[flag] ? "on" : "off");
		}
		break;
	}
}

void keyup(unsigned char key, int x, int y){
	switch (key) {
	case 'a': case 'd':
		MyScene.cam.side = 0;
		break;
	case 'q': case 'e':
		MyScene.cam.alt = 0;
		break;
	case 'w': case 's':
		MyScene.cam.forward = 0;
		break;
	default:
		break;
	}
}

void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void drawInfo();

Timer fpsTimer(100);
char infoString[100];
void drawFPS();

// entry point
int main(int argc, char** argv){
    glutInit(&argc, argv);

    // couches du framebuffer utilisees par l'application
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    // position et taille de la fenetre
    glutInitWindowPosition(200, 10);
    glutInitWindowSize(WINDOW_RES_X, WINDOW_RES_Y);
    glutCreateWindow("Realtime Raytracing");

    // init textures
	printf("Generating textures...");
    char* buf = new char[1024 * 1024 * 3];
    glGenTextures(2, textures);

    // texture 1
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB,
    GL_UNSIGNED_BYTE, buf);

    // texture 2
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB,
    GL_UNSIGNED_BYTE, buf);

    delete[] buf;

    // cablage des callback
    glutReshapeFunc(reshape);
    glutSetKeyRepeat(false);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyup);
    glutSpecialFunc(specialKeyboard);
    glutSpecialUpFunc(specialKeyboardUp);
    glutDisplayFunc(display);
    glutMouseFunc(mouseFunc);
    glutIdleFunc(drawInfo);

	glutMotionFunc(mouseMotionFunc);
	glutPassiveMotionFunc(mouseMotionFunc);
	int ret;
    try {
    	 ret = init(argc, argv);
    } catch (const char* s) {
    	printf("%s\n", s);
    	exit(-1);
    }
    if(ret == 255)
        return 0;
    if(ret > 0)
        return ret;

	// main loop
    glutMainLoop();

    return 0;
}

// draw fps
void drawInfo(){
    MyScene.update();
    glutPostRedisplay();

    float fps = 1. / fpsTimer.avg();
    fpsTimer.updateLastDisplay();
    sprintf(infoString, "%06.1f fps - Current object : %s", fps,
            MyScene.object->getName().c_str());

    int i = 0;
    while(infoString[i] != '\0'){
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, infoString[i++]);
    }
}

// display
clock_t ticks;
void display(void){

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    // Effacer tout
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // la couleur et le z

    glLoadIdentity();  // repere camera

    if(isDrawingTexture || isRealtimeRaytracing){
        const static GLdouble viewport[] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
           -2,-2,-4, 1
        };

        glMultMatrixd(viewport);
        drawTexture(activeTexIndex);

        // reset view
        glLoadIdentity();
        viewTransform();

        // swap buffers; draw on back buffer
        if(isRealtimeRaytracing){
            clock_t start = clock();
            startRayTracing(!activeTexIndex, false);
            ticks = clock() - start;
            activeTexIndex = !activeTexIndex;
        }else{
            startRayTracing(activeTexIndex, needRebuild);
            if(needRebuild == true){
                int millis = (int)(ticks * 1000. / CLOCKS_PER_SEC);
                long long expected = millis;
                expected *= RAYTRACE_RES_X / PREVIEW_RES_X;
                expected *= RAYTRACE_RES_Y / PREVIEW_RES_Y;
                expected *= MSAA / PREVIEW_MSAA;
                expected *= MSAA / PREVIEW_MSAA;
                if(expected < 1000)
                    printf("will take %d milliseconds\n", (int)expected);
                else if(expected < 1000 * 60)
                    printf("will take %d seconds\n", (int)(expected / 1000));
                else if(expected < 1000 * 60 * 60)
                    printf("will take %d minutes\n", (int)(expected / 1000 / 60));
                else if(expected < 1000 * 60 * 60 * 24){
                    printf("RENDERING WILL TAKE LONG!\n");
                    printf("will take %d hour\n", (int)(expected / 1000 / 60 / 60));
                }else if(expected < (long long)1000 * 60 * 60 * 24 * 365){
                    printf("RENDERING WILL TAKE VERY LONG!\n");
                    printf("will take %d days\n", (int)(expected / 1000 / 60 / 60 / 24));
                }
                else if (expected < (long long)1000 * 60 * 60 * 24 * 365 * 1000) {
                    printf("RENDERING will take years!\n");
                    printf("will take %d year\n", (int)(expected / 1000 / 60 / 60 / 24 / 365 / 1000));
                }
                else if (expected < (long long)1000 * 60 * 60 * 24 * 365 * 1000 * 1000) {
                    printf("RENDERING will take thousands of years!\n");
                    printf("will take %d millenia\n", (int)(expected / 1000 / 60 / 60 / 24 / 365 / 1000 / 1000));
                }
                else if (expected < (long long)1000 * 60 * 60 * 24 * 365 * 1000 * 1000) {
                    printf("RENDERING will take millions of years!\n");
                    printf("will take %d million years\n", (int)(expected / 1000 / 60 / 60 / 24 / 365 / 1000 / 1000));
                }
                else if (expected < (float)1000 * 60 * 60 * 24 * 365 * 1000 * 1000 * 1000) {
                    printf("If the dinosaurs were alive when you started rendering, it would be ready now.\n");
                    printf("will take %d billion years\n", (int)(expected / 1000 / 60 / 60 / 24 / 365 / 1000 / 100));
                }
                else {
                    printf("THIS IS MADNESS!\n");
                    printf("will take %s seconds\n", "<overflow error>");
                }
                needRebuild = false;
            }
        }
    } else {
        viewTransform(); // origine et orientation de la scene
        MyScene.draw();
		if (g_flags[DEBUG])
            MyScene.debugDraw();
    }

    glutSwapBuffers();
    glPopAttrib();
}
