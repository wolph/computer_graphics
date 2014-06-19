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

class Scene {
private:
	vector<Light> lights;
	Mesh mesh;
public:
	Scene();
};