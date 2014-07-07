/**
* xXx_octtree_2014_MLG_noscope_xXx

*/
#pragma once
#include "Mesh.hpp"
#include "constants.hpp"
#include "Vec3D.hpp"

struct AABB {
	// the subtrees, if any
	AABB** sub;
    vector<POLY> leaves; // triangle indices

	// top left up corner
	SVEC pos;

	// half dimension!
	float radius;

	AABB();
	AABB(POS pos, float radius);
	~AABB();

	bool hit(RAY ray);
	void split();
	int follow(VEC vec);
	float collide(RAY ray, OUT POLY* p);
	bool collidePlane(RAY ray, int axis);
};

class Tree {
private:
    unsigned int max_depth;
	void calcSize(Mesh& mesh);

public:
	AABB* root;

	void build(Mesh& mesh);
	void add(POLY p);
	void add(Triangle& tr);
	float collide(RAY ray, OUT POLY* p);
};
