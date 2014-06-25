/**
* xXx_octtree_2014_MLG_noscope_xXx

*/
#pragma once
#include "Mesh.hpp"
#define MAX_DEPTH 10

struct AABB {
	// the subtrees, if any
	AABB** sub;
    std::vector<Triangle*> leaves; // triangle indices

	// top left up corner
	Vec3Df pos;

	// half dimension!
	float radius;

	AABB();
	AABB(Vec3Df& pos, float radius);
	~AABB();

	bool hit(const Vec3Df& orig, const Vec3Df& dir);
	void split();
	int follow(const Vec3Df& v);
	float collide(const Vec3Df& orig, const Vec3Df& dir, Triangle** tr);
	bool collidePlane(int axis, const Vec3Df& orig, const Vec3Df& dir);
};

class Tree {
private:
	void calcSize(Mesh& mesh);

public:
	AABB* root;

	void build(Mesh& mesh);
	void add(Triangle& tr);
	float collide(const Vec3Df& orig, const Vec3Df& dir, Triangle** tr);
};
