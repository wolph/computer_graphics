#include "main.hpp"

void drawAxes(float length){
    glDisable(GL_LIGHTING);

    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(length, 0, 0);

    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, length, 0);

    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, length);
    glEnd();
    glEnable(GL_LIGHTING);

}

/**
 * draw a full-screen texture
 */
void drawTexture(){
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
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

/**
 * Appel des différentes fonctions de dessin
 */
void draw(){

    switch(type){
        case TRIANGLE:
            glutSolidSphere(1, 10, 10);
            drawAxes(1);
            break;
        case MODEL: {
            MyMesh.draw();
            //glBegin(GL_TRIANGLES);

            //for (unsigned int i=0;i<MyMesh.triangles.size();++i)
            //{
            //	glColor3f(MyMesh.materials[MyMesh.triangleMaterials[i]].Kd()[0], MyMesh.materials[MyMesh.triangleMaterials[i]].Kd()[1], MyMesh.materials[MyMesh.triangleMaterials[i]].Kd()[2]);
            //	glVertex3f(MyMesh.vertices[MyMesh.triangles[i].v[0]].p[0], MyMesh.vertices[MyMesh.triangles[i].v[0]].p[1], MyMesh.vertices[MyMesh.triangles[i].v[0]].p[2]);
            //	glVertex3f(MyMesh.vertices[MyMesh.triangles[i].v[1]].p[0], MyMesh.vertices[MyMesh.triangles[i].v[1]].p[1], MyMesh.vertices[MyMesh.triangles[i].v[1]].p[2]);
            //	glVertex3f(MyMesh.vertices[MyMesh.triangles[i].v[2]].p[0], MyMesh.vertices[MyMesh.triangles[i].v[2]].p[1], MyMesh.vertices[MyMesh.triangles[i].v[2]].p[2]);
            //}
            //glEnd();
            break;
        }
        default:
            drawAxes(1); // Par défaut
            break;
    }

    yourDebugDraw();
}

void animate(){
    MyCameraPosition = getCameraPosition();
    glutPostRedisplay();
}

void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);

/**
 * Programme principal
 */
int main(int argc, char** argv){
    glutInit(&argc, argv);

    // couches du framebuffer utilisees par l'application
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    // position et taille de la fenetre
    glutInitWindowPosition(200, 100);
    glutInitWindowSize(WindowSize_X, WindowSize_Y);
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

    // init texture
    char* buf = new char[1024 * 1024 * 3];
    for(int i = 0;i < 1024 * 1024 * 3;i++)
        buf[i] = rand() % 255;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
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
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutMouseFunc(tbMouseFunc);    // traqueboule utilise la souris
    glutMotionFunc(tbMotionFunc);  // traqueboule utilise la souris
    glutIdleFunc(animate);

    int ret = init(argc, argv);
    if(ret == 255)return 0;
    if(ret > 0)return ret;

    // lancement de la boucle principale
    glutMainLoop();

    return 0;  // instruction jamais exécutée
}

/**
 * Fonctions de gestion opengl à ne pas toucher
 */
// Actions d'affichage
// Ne pas changer
void display(void){
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    // Effacer tout
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // la couleur et le z

    glLoadIdentity();  // repere camera

    if(isDrawingTexture){
        GLdouble tb_matrix2[] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -2, -2, -4,
                1};
        glMultMatrixd(tb_matrix2);
        drawTexture();
    }else{
        tbVisuTransform(); // origine et orientation de la scene
        draw();
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
    printf("key %d pressed at %d,%d\n", key, x, y);
    fflush(stdout);
    switch(key){
        case 't':
            isDrawingTexture = !isDrawingTexture;
            break;
        case 'L':
            MyLightPositions.push_back(getCameraPosition());
            break;
        case 'l':
            MyLightPositions[MyLightPositions.size() - 1] = getCameraPosition();
            break;
        case 'r': {
            startRayTracing();
            isDrawingTexture = 1;
            break;
        }
        case 27:     // touche ESC
            exit(0);
    }

    yourKeyboardFunc(key, x, y);
}

