/**
* Contains a scene with light sources and meshes
*/
#ifndef SCENE_HPP
#define SCENE_HPP

#include "Vec3D.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"
#include "Tree.hpp"
#include "RayTracer.hpp"
#include <vector>

#pragma pack(push,1)
struct PolyIdx {
	unsigned short a, b, c;
};
#pragma pack(pop)

class Model {
public:
	vector<Vec3Df> vertices;
	vector<PolyIdx> polygons;
	void load(string file);
};

static const char* nil = 0;

class Object {
protected:
	inline Object() : mesh((Mesh&) *nil) {}
public:
    Tree tree;
	Mesh& mesh;
	Vec3Df pos;
	Vec3Df vel;
	inline Object(Vec3Df& pos, Mesh& mesh) : mesh(mesh), pos(pos), vel(0, 0, 0) {
	    tree.build(mesh);
    }
	void draw();
	float raytrace(RAY ray, OUT POS impact, OUT POS normal);
};

class Scene {
public:
	vector<Vec3Df> lights;
	vector<Object*> objects;
    Object* object;
	Camera cam;
	float floorheight;

	inline Scene(){};
	void load(string path);
	void draw();
	void debugDraw();
	void update();
	void add(Object* object);
	void addLightPoint(Vec3Df& lightPos);
	bool raytrace(RAY ray, OUT POS impact, OUT VEC normal);

    Object* nextObject();
    Object* prevObject();

private:
    unsigned int objectIndex;
};
#endif