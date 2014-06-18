#include "Tree.hpp"
#include <cstdio>

AABB::AABB() : sub(0), radius(-1) { }

AABB::AABB(Vec3Df& pos, float radius) : pos(pos), radius(radius), sub(0) {
}

AABB::~AABB() {
	// can safeley delete the null pointer
	delete[] sub;
}

int AABB::follow(Vec3Df& v) {
	int axis = 0;
	for (int a = 0; a < 3; a++) {
		if (v.p[a] > pos.p[a] + radius)
			axis |= 1 << a;
	}

	return axis;
}

void AABB::split() {
	// if already subdivided, return
	if (sub)
		return;

	// make leaves
	sub = new AABB*[8];
	for (int x = 0; x < 2; x++)
	for (int y = 0; y < 2; y++)
	for (int z = 0; z < 2; z++) {
		Vec3Df subpos(pos + Vec3Df(x * radius, y * radius, z * radius));
		float subradius = radius * 0.5f;
		
		sub[z * 4 + y * 2 + x] = new AABB(subpos, subradius);
	}
}

bool AABB::collidePlane(int axis, Ray& ray) {
	// check axis plane
	float v1 = pos.p[axis];
	if (ray.dir.p[axis] > 0)
		v1 += 2 * radius;

	// factor
	float a = (v1 - ray.orig.p[axis]) / ray.dir.p[axis];

	// other axis
	int axis2 = (axis + 1) % 3;
	int axis3 = (axis + 2) % 3;

	// other axis values
	float v2 = a * ray.dir.p[axis2] + ray.orig.p[axis2];
	float v3 = a * ray.dir.p[axis3] + ray.orig.p[axis3];

	// check 2D aabb
	if (pos.p[axis2] < v2 && v2 < pos.p[axis2] + 2 * radius)
		if (pos.p[axis3] < v3 && v3 < pos.p[axis3] + 2 * radius)
			return true;
	return false;
}

float AABB::collide(Ray& ray, Triangle** out) {
	// check hit with this cube
	bool hit = false;
	for (int i = 0; i < 3; i++) {
		if (collidePlane(i, ray)) {
			hit = true;
			break;
		}
	}

	// return when no hit
	if (!hit)
		return 0;

	// current closest triangle
	float shortest = 1e10f;
	Triangle* res = 0;

	// check with leaves
	for (int i = 0; i < leaves.size(); i++) {
		float dist = ray.intersect(leaves[i]);
		if (dist < shortest) {
			shortest = dist;
			res = leaves[i];
		}
	}

	// check with subnodes
	// TODO: skip irrelevant subnodes
	for (int i = 0; i < 8; i++) {
		Triangle* res2;
		float dist = sub[i]->collide(ray, &res2);
		if (dist < shortest) {
			res = res2;
			shortest = dist;
		}
	}

	// hit
	*out = res;
	return shortest;

	//printf("(%f, %f)\n", y, z);

	return 0;
}

void Tree::calcSize(Mesh& mesh) {
	Vec3Df p1; // min
	Vec3Df p2; // max

	for (unsigned int i = 0; i < mesh.triangles.size(); i++) { // triangle
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

	for (unsigned int i = 0; i < mesh.triangles.size(); i++)
		add(mesh.triangles[i]);

	//Ray ray = Ray(Vec3Df(0, 0, 0), Vec3Df(-2, -5, 0), Vec3Df(-1, 0, 0));
	//Triangle* tr = root->collide(ray);
}

void Tree::add(Triangle& tr) {
	bool same = true;
	AABB* current = root;
	int a0, a1, a2;

	int depth = 0;
	while (depth < MAX_DEPTH) {
		a0 = current->follow(tr.vertices[0].p);
		a1 = current->follow(tr.vertices[1].p);
		a2 = current->follow(tr.vertices[2].p);

		//printf("follow: %d, %d, %d\n", a0, a1, a2);

		if (a0 != a1 || a1 != a2)
			break;

		current->split();
		current = current->sub[a0];
		depth++;
	}

	//printf("Depth: %d\n\n", depth);

	current->leaves.push_back(&tr);
}

float Tree::collide(Ray& ray, Triangle** out) {
	return root->collide(ray, out);
}