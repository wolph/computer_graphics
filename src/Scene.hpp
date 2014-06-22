/**
* Contains a scene with light sources and meshes
*/
#include "Vec3D.hpp"
#include "Mesh.hpp"
#include "Tree.hpp"
#include <vector>

#pragma pack(push,1)
struct PolyIdx {
	unsigned short a, b, c;
};
#pragma pack(pop)

class Model {
public:
	std::vector<Vec3Df> vertices;
	std::vector<PolyIdx> polygons;
	void load(std::string file);
};

class Object {
public:
    Tree tree;
	Mesh& mesh;
	Vec3Df pos;
	Vec3Df vel;
	inline Object(Vec3Df& pos, Mesh& mesh) : mesh(mesh), pos(pos), vel(0, 0, 0) {
	    tree.build(mesh);
    }
	void draw();
	float raytrace(const Vec3Df& orig, const Vec3Df& dir, Triangle** tr);
};

class Scene {
public:
	std::vector<Vec3Df> lights;
	std::vector<Object*> objects;
    Object* object;

	inline Scene(){};
	void load(string path);
	void draw();
	void debugDraw();
	void update();
	void add(Object* object);
	void addLightPoint(Vec3Df& lightPos);
	float raytrace(const Vec3Df& orig, const Vec3Df& dir, Triangle** tr, Object** obj);

    Object* nextObject(){
        objectIndex = (objectIndex + 1) % objects.size();
        return object = objects[objectIndex];
    }

    Object* prevObject(){
        objectIndex = (unsigned int)(objectIndex ? objectIndex : objects.size()) - 1;
        return object = objects[objectIndex];
    }
private:
    unsigned int objectIndex;
};
