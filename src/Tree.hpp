/**
* xXx_octtree_2014_MLG_noscope_xXx

*/
#pragma once
#include "Mesh.hpp"
#include "Ray.hpp"
#include "constants.hpp"

struct AABB {
	// the subtrees, if any
	AABB** sub;
	vector<Triangle*> leaves; // triangle indices

	// top left up corner
	Vec3Df pos;

	// half dimension!
	float radius;

	AABB();
	AABB(Vec3Df& pos, float radius);
	~AABB();

	void split();
	int follow(Vec3Df& v);
	float collide(Ray& ray, Triangle** out);
	bool collidePlane(int axis, Ray& ray);
};

class Tree {
private:
	void calcSize(Mesh& mesh);

public:
	AABB* root;

	void build(Mesh& mesh);
	void add(Triangle& tr);
	float collide(Ray& ray, Triangle** out);
};