#include <cstdlib>
#include <cmath>
#include <ctime>
#include <GL/glut.h>
#include <assert.h>
#include <cstdlib>

#include "main.hpp"
#include "Scene.hpp"

// don't define anything in headers! only declare it!!!1!one!
Vec3Df MyCameraPosition;

std::vector<Vec3Df> MyLightPositions;

// double buffered
unsigned int textures[2];
unsigned int activeTexIndex = 0;
unsigned int isDrawingTexture = 0;
unsigned int isRealtimeRaytracing = 0;
Tree MyTree;
Scene MyScene;

// options
extern bool g_phong;

bool needRebuild = false; // if the raytrace needs to be built

/**
 * draw a full-screen texture
 */
void drawTexture(int texIndex){
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[texIndex]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(0, 0, 0);
    glTexCoord2f(1, 0);
    glVertex3f(4, 0, 0);
    glTexCoord2f(1, 1);
    glVertex3f(4, 4, 0);
    glTexCoord2f(0, 1);
    glVertex3f(0, 4, 0);
    glEnd();
}

void animate() {
    MyCameraPosition = getCameraPosition();
    glutPostRedisplay();
}

void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);

// entry point
int main(int argc, char** argv){
    glutInit(&argc, argv);

    // couches du framebuffer utilisees par l'application
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    // position et taille de la fenetre
    glutInitWindowPosition(200, 100);
    glutInitWindowSize(WINDOW_RES_X, WINDOW_RES_Y);
    glutCreateWindow(argv[0]);

    // Initialisation du point de vue
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, -4);
    glRotatef(30, -1, 1, 1);
    tbInitTransform();     // initialisation du point de vue
    tbHelp();                      // affiche l'aide sur la traqueboule
    MyCameraPosition = getCameraPosition();
    //
    // Active la lumière
    // Pour la partie
    // ECLAIRAGE

    glEnable( GL_LIGHTING);
    glEnable( GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    int LightPos[4] = {0, 0, 3, 1};
    glLightiv(GL_LIGHT0, GL_POSITION, LightPos);
    //glMaterialiv(GL_FRONT_AND_BACK,GL_SPECULAR,MatSpec);
    //glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,10);

    glEnable(GL_NORMALIZE);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // Details sur le mode de tracé
    glEnable( GL_DEPTH_TEST);            // effectuer le test de profondeur
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);
    glShadeModel(GL_SMOOTH);

    // init textures
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
	glutSetKeyRepeat(true);
    glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyup);
    glutDisplayFunc(display);
    glutMouseFunc(tbMouseFunc);    // traqueboule utilise la souris
    glutMotionFunc(tbMotionFunc);  // traqueboule utilise la souris
    glutIdleFunc(animate);

    int ret = init(argc, argv);
    if(ret == 255)
        return 0;
    if(ret > 0)
        return ret;

    // lancement de la boucle principale
    glutMainLoop();

    return 0;  // instruction jamais exécutée
}

// draw fps
void drawFPS(){
    clock_t diff = clock() - lastFrameTime;
    lastFrameTime = clock();

    if((0. + lastFrameTime - lastFPSRenderTime) / CLOCKS_PER_SEC > .01){
        lastFPSRenderTime = lastFrameTime;
        float fps = (1. / diff) * CLOCKS_PER_SEC;
        if(isRealtimeRaytracing)
            fps *= THREADS;
        sprintf(screenFPS, "%.1f fps", fps);
    }

    int i = 0;
    while(screenFPS[i] != '\0'){
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, screenFPS[i++]);
    }
}

// display
clock_t ticks;
void display(void) {

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    // Effacer tout
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // la couleur et le z
    drawFPS();

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
        tbVisuTransform();

        // swap buffers; draw on back buffer
		if (isRealtimeRaytracing){
			clock_t start = clock();
            startRayTracing(!activeTexIndex, false);
			ticks = clock() - start;
            activeTexIndex = !activeTexIndex;
        } else {
            if (needRebuild == true){
				int millis = ticks * 1000 / CLOCKS_PER_SEC;
				long long expected = millis;
				expected *= RAYTRACE_RES_X / PREVIEW_RES_X;
				expected *= RAYTRACE_RES_Y / PREVIEW_RES_Y;
				expected *= MSAA / PREVIEW_MSAA;
				expected *= MSAA / PREVIEW_MSAA;
				if (expected < 1000)
					printf("will take %d milliseconds\n", expected);
				else if (expected < 1000 * 60)
					printf("will take %d seconds\n", expected / 1000);
				else if (expected < 1000 * 60 * 60)
					printf("will take %d minutes\n", expected / 1000 / 60);
				else if (expected < 1000 * 60 * 60 * 24) {
					printf("RENDERING WILL TAKE LONG!\n");
					printf("will take %d hour\n", expected / 1000 / 60 / 60);
				} else if (expected < 1000 * 60 * 60 * 24 * 365) {
					printf("RENDERING WILL TAKE VERY LONG!\n");
					printf("will take %d days\n", expected / 1000 / 60 / 60 / 24);
				}
				else if (expected < 1000 * 60 * 60 * 24 * 365 * 1000) {
					printf("RENDERING will take thousands of years!\n");
					printf("will take %d millenia\n", expected / 1000 / 60 / 60 / 24 / 365 / 1000);
				}
				else if (expected < 1000 * 60 * 60 * 24 * 365 * 1000 * 1000) {
					printf("RENDERING will take millions of years!\n");
					printf("will take %d millenia\n", expected / 1000 / 60 / 60 / 24 / 365 / 1000 / 1000);
				}
				else if (expected < 1000 * 60 * 60 * 24 * 365 * 1000 * 1000 * 100) {
					printf("If the dinosaurs were alive when you started rendering, it would be ready now.\n");
					printf("will take %d eras\n", expected / 1000 / 60 / 60 / 24 / 365 / 1000 / 100);
				}
				else {
					printf("THIS IS MADNESS!\n");
					printf("will take %s seconds\n", "<overflow error>");
				}
                startRayTracing(activeTexIndex, true);
                needRebuild = false;
            }
        }
		drawFPS();
    } else {
        tbVisuTransform(); // origine et orientation de la scene
		MyScene.draw();
		yourDebugDraw();
    }

    glutSwapBuffers();
    glPopAttrib();
}
// pour changement de taille ou desiconification
void reshape(int w, int h){
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho (-1.1, 1.1, -1.1,1.1, -1000.0, 1000.0);
    gluPerspective(50, (float)w / h, 1, 10);
    glMatrixMode(GL_MODELVIEW);
}

// prise en compte du clavier
void keyboard(unsigned char key, int x, int y){
    switch(key){
		case '1':
			g_phong = !g_phong;
			break;
        case 't':
            isDrawingTexture = 0;
            isRealtimeRaytracing = 0;
            break;
        case 'L':
            MyLightPositions.push_back(getCameraPosition());
            break;
        case 'l':
            MyLightPositions[MyLightPositions.size() - 1] = getCameraPosition();
            break;
        case 'r':
            needRebuild = true;
            isDrawingTexture = 1;
            isRealtimeRaytracing = 0;
            break;
        case 'b':
            cout << "Using " << THREADS << " threads and resolution of "
                    << PREVIEW_RES_X << "x" << PREVIEW_RES_Y << endl;
            isRealtimeRaytracing = 1;
            isDrawingTexture = 0;
            break;
        case 27:     // touche ESC
            exit(0);
    }

	yourKeyboardPress(key, x, y);
}

void keyup(unsigned char key, int x, int y) {
	yourKeyboardRelease(key, x, y);
}

