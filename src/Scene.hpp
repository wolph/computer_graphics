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
	virtual void draw();
	virtual float raytrace(const Vec3Df& orig, const Vec3Df& dir, Vec3Df* impact, Vec3Df* normal, Material** mat);
};

class Sphere : public Object {
	float radius;
	Vec3Df center;
public:
	inline Sphere(Vec3Df& center, float radius) : center(center) {
		this->radius = radius;
	}
	void draw();
	float raytrace(const Vec3Df& orig, const Vec3Df& dir, Vec3Df* impact, Vec3Df* normal, Material** mat);
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
	bool raytrace(const Vec3Df& orig, const Vec3Df& dir, Vec3Df* impact,
                  Vec3Df* normal, Material** mat, Object** obj);

    Object* nextObject();
    Object* prevObject();

private:
    unsigned int objectIndex;
};