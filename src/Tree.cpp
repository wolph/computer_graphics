#include "Tree.hpp"
#include <cstdio>

AABB::AABB() : bin(0), sub(0), radius(-1) { }

AABB::AABB(Vec3Df& pos, float dim) : pos(pos), radius(radius), sub(0) {
	bin = new Vertex[BINSIZE];
	len = 0;
}

AABB::~AABB() {
	// can safeley delete the null pointer
	delete[] bin;
	delete[] sub;
}

AABB* AABB::follow(Vec3Df& v) {
	if (!sub)
		return 0;

	int axis = 0;
	for (int a = 0; a < 3; a++) {
		if (v.p[a] > pos.p[a] + radius)
			axis |= 1 << a;
	}

	return sub[axis];
}

void AABB::split() {
	// make leaves
	sub = new AABB*[8];
	for (int x = 0; x < 2; x++)
	for (int y = 0; y < 2; y++)
	for (int z = 0; z < 2; z++) {
		Vec3Df subpos(pos + Vec3Df(x * radius, y * radius, z * radius));
		float subradius = radius * 0.5f;
		
		sub[z * 4 + y * 2 + x] = new AABB(subpos, subradius);
	}

	// spread this bin
	for (int i = 0; i < BINSIZE; i++) {
		AABB* child = follow(bin[i].p);
		child->bin[child->len++] = bin[i];
	}

	// clean
	delete[] bin;
}

void Tree::calcSize(Mesh& mesh) {
	Vec3Df p1; // min
	Vec3Df p2; // max

	for (int i = 0; i < mesh.triangles.size(); i++) { // triangle
		for (int v = 0; v < 3; v++) { // vertex
			for (int d = 0; d < 3; d++) { // dimension
				const float dim = mesh.triangles[i].vertices[v].p.p[d];
				if (dim < p1.p[d])
					p1.p[d] = dim;
				if (dim > p2.p[d])
					p2.p[d] = dim;
			}
		}
	}
	printf("min: (%.1f,%.1f,%.1f)\n", p1.p[0], p1.p[1], p1.p[2]);
	printf("max: (%.1f,%.1f,%.1f)\n", p2.p[0], p2.p[1], p2.p[2]);

	Vec3Df dim = p2 - p1;
	float sdim = dim.p[0];
	sdim = max(sdim, dim.p[1]);
	sdim = max(sdim, dim.p[2]);

	root = new AABB(p1, sdim * 0.5f);
}

void Tree::build(Mesh& mesh) {
	printf("Building tree!\n");
	calcSize(mesh);

	//for (int i = 0; i < mesh.triangles.size(); i++)
	//	add(mesh.triangles[i]);
}

void Tree::add(Triangle& tr) {
	for (int i = 0; i < 3; i++) { // vertex
		AABB* current = root;
		Vec3Df& v = tr.vertices[i].p;

		// get fitting sub AABB
		int id = 0;
		while (current->sub)
		{current = current->follow(v); id++;}

		// check if it's full
		if (current->len == BINSIZE) {
			printf("splitting @ depth %d\n", id);
			current->split();
			current = current->follow(v);
		}
		printf("adding\n");// %d\n", )
		current->bin[current->len++] = tr.vertices[i];
	}
}