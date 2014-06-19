/**
* Contains a scene with light sources and meshes
*/
#include "Vec3D.hpp"
#include "Mesh.hpp"
#include <vector>
using namespace std;

// a dot light
class Light {
public:
	Vec3Df pos;
};

#pragma pack(push,1)
struct PolyIdx {
	unsigned short a, b, c;
};
#pragma pack(pop)

class Model {
public:
	vector<Vec3Df> vertices;
	vector<PolyIdx> polygons;
};

class Scene {
private:
	vector<Light> lights;
	Mesh mesh;
public:
	Scene();
};