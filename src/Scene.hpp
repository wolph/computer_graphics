/**
* Contains a scene with light sources and meshes
*/
#include "Vec3D.hpp"
#include "Mesh.hpp"
#include "Tree.hpp"
#include <vector>
using namespace std;

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

class Object {
	Tree tree;
	Mesh mesh;
public:
	Vec3Df pos;
	Vec3Df vel;
	Object(Vec3Df& pos, Mesh mesh);
	void draw();
	float raytrace(Ray& ray, Triangle** tr);
};

class Scene {
private:
	vector<Vec3Df> lights;
	vector<Object*> objects;
public:
	Scene();
	void draw();
	void update();
	void add(Object* object);
	void addLightPoint(Vec3Df& lightPos);
	float raytrace(Ray& ray, Triangle** tr);
};