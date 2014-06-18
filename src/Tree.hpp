/**
* xXx_octtree_2014_MLG_noscope_xXx

*/
#pragma once
#include "Mesh.hpp"

#define BINSIZE 1

struct AABB {
	// the subtrees, if any
	AABB** sub;
	Vertex* bin;
	int len;

	// top left up corner
	Vec3Df pos;

	// half dimension!
	float radius;

	AABB();
	AABB(Vec3Df& pos, float radius);
	~AABB();

	void split();
	AABB* follow(Vec3Df& v);
};

class Tree {
private:
	void calcSize(Mesh& mesh);

public:
	AABB* root;

	void build(Mesh& mesh);
	void add(Triangle& tr);
};