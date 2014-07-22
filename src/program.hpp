#pragma once
#include "scene.hpp"
#include "image.hpp"
#include "texture.hpp"
#include "threading.hpp"

#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28

// flag types
enum { SHADOW, CHECK, DEBUG, AMBIENT, DIFFUSE, SPECULAR, REFLECT, REFRACT, OCCLUSION, PHONG };

static const char* flagstrs[10] = {
	"shadow", "checkerboard", "debug", "ambient", "diffuse", "specular", "reflect", "refract",
	"occlusion", "phong"
};

class Program {
public:
	int activeTexIndex;
	bool flags[10];
	Scene scene;
	ThreadPool pool;
	Image preview;
	Texture textures[2]; // front & backbuffer

	Program();

	void OnInit();
	void OnResize(int w, int h);
	void OnKeyPress(int key);
	void OnKeyRelease(int key);
	void OnDraw();
};