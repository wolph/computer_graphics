#include "program.hpp"
#include "threading.hpp"
#include <gl/glut.h>
#include <thread>
using namespace std;

Program::Program() : pool(getNumberOfCores()), activeTexIndex(0), preview(1024, 1024) {
	// reset flags
	for (int i = 0; i < 10; i++)
		flags[i] = false;

	// init textures
	textures[0].init();
	textures[1].init();

	// load the scene
	scene.load("mesh/scene.txt");
}

void Program::OnDraw() {
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	// Effacer tout
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // la couleur et le z

	glLoadIdentity();  // repere camera

	const static GLdouble viewport[] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-2, -2, -4, 1
	};

	glMultMatrixd(viewport);

	// reset view
	glLoadIdentity();
	//viewTransform(); ->
	scene.cam.BuildMatrix();
	glMultMatrixf(scene.cam.viewmat);

	// swap buffers; draw on back buffer
	clock_t start = clock();
	textures[activeTexIndex].draw();
	render(preview);
	textures[activeTexIndex].upload(preview);
	activeTexIndex = !activeTexIndex;

	int ticks = clock() - start;
	printf("%d ms\n", ticks);

	glutSwapBuffers();
	glPopAttrib();
}

void Program::OnResize(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, (float)w / h, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

void Program::OnKeyPress(int key) {
	switch (key){
	case VK_UP:
		scene.cam.rotateY = 1;
		break;
	case VK_DOWN:
		scene.cam.rotateY = -1;
		break;
	case VK_RIGHT:
		scene.cam.rotateX = 1;
		break;
	case VK_LEFT:
		scene.cam.rotateX = -1;
		break;
	case 'L':
		scene.addLightPoint(scene.cam.pos);
		break;
	case 'l':
		scene.lights[scene.lights.size() - 1] = scene.cam.pos;
		break;

		/* Escape */
	case 27:
		// TODO: fix this horrible hack to stop segfaults when exiting
		this_thread::sleep_for(chrono::milliseconds(1000));
		exit(0);

		/* Movement */
	case 'a': scene.cam.side = 1;	break;
	case 'd': scene.cam.side = -1; break;
	case 'q': scene.cam.alt = 1; break;
	case 'e': scene.cam.alt = -1; break;
	case 'w': scene.cam.forward = 1; break;
	case 's': scene.cam.forward = -1; break;

	default:
		// flags
		if ('0' <= key && key <= '9') {
			int flag = key - '0';
			flags[flag] = !flags[flag];
			printf("%s := %s\n", flagstrs[flag], flags[flag] ? "on" : "off");
		}
		break;
	}
}

void Program::OnKeyRelease(int key) {
	switch (key){
	case VK_UP:
	case VK_DOWN:
		scene.cam.rotateY = 0;
		break;
	case VK_RIGHT:
	case VK_LEFT:
		scene.cam.rotateX = 0;
		break;
	case 'a': case 'd':
		scene.cam.side = 0;
		break;
	case 'q': case 'e':
		scene.cam.alt = 0;
		break;
	case 'w': case 's':
		scene.cam.forward = 0;
		break;
	default:
		break;
	}
}