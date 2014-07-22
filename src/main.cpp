#ifdef _WIN32
#include <Windows.h>
#endif

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
#include "program.hpp"

// one last time...
Program* prog;

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
		if (arg != 0){
			//alternateX = stoi(arg);
		}
	}

	if (options[RAYTRACE_Y]) {
		const char* arg = options[RAYTRACE_Y].last()->arg;
		if (arg != 0){
			//alternateY = stoi(arg);
		}
	}

	return 0;
}

void display(void) { prog->OnDraw(); }
void reshape(int w, int h) { prog->OnResize(w, h); }
void keyboard(unsigned char key, int x, int y) { prog->OnKeyPress(key); }
void keyup(unsigned char key, int x, int y) { prog->OnKeyRelease(key); }
void specialKeyboard(int key, int x, int y) { prog->OnKeyPress(key); }
void specialKeyboardUp(int key, int x, int y) { prog->OnKeyRelease(key); }
//void drawInfo();

// entry point
int main(int argc, char** argv){
    glutInit(&argc, argv);

    // couches du framebuffer utilisees par l'application
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    // position et taille de la fenetre
    glutInitWindowPosition(200, 10);
    glutInitWindowSize(WINDOW_RES_X, WINDOW_RES_Y);
    glutCreateWindow("Realtime Raytracing");

	Program program;
	prog = &program;

    // call back
    glutReshapeFunc(reshape);
    glutSetKeyRepeat(false);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyup);
    glutSpecialFunc(specialKeyboard);
    glutSpecialUpFunc(specialKeyboardUp);
    glutDisplayFunc(display);
    glutMouseFunc(mouseFunc);
   // glutIdleFunc(drawInfo);

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
/*void drawInfo(){
    MyScene.update();
    glutPostRedisplay();

    float fps = 1. / fpsTimer.avg();
    fpsTimer.updateLastDisplay();
  //  sprintf(infoString, "%06.1f fps - Current object : %s", fps,
//            MyScene.object->getName().c_str());

    int i = 0;
    while(infoString[i] != '\0'){
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, infoString[i++]);
    }
}*/