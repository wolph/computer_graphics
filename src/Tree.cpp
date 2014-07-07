#include "Tree.hpp"
#include <cstdio>

AABB::AABB() : sub(0), radius(-1) { }

AABB::AABB(Vec3Df& pos, float radius) : sub(0), pos(pos), radius(radius) {
}

AABB::~AABB() {
	// can safeley delete the null pointer
	delete[] sub;
}

int AABB::follow(const Vec3Df& v) {
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
	for (int x = 0; x < 2; x++){
		for (int y = 0; y < 2; y++){
			for (int z = 0; z < 2; z++){
				Vec3Df subpos(pos + Vec3Df(x * radius, y * radius, z * radius));
				float subradius = radius * 0.5f;

				sub[z * 4 + y * 2 + x] = new AABB(subpos, subradius);
			}
		}
	}
}

bool AABB::collidePlane(int axis, const Vec3Df& orig, const Vec3Df& dir) {
	// check axis plane
	float v1 = pos.p[axis];

	// flip
	bool flip = (dir.p[axis] < 0);
	//if (dir.p[axis] > 0 && orig.p[axis] > v1) flip = !flip;
	//if (dir.p[axis] < 0 && orig.p[axis] < v1 + 2 * radius) flip = !flip;
	v1 += flip * 2 * radius;

	// skip
	if ((v1 - orig.p[axis] - flip * 2 * radius) / dir.p[axis] < 0)
		return false;

	// factor
	float a = (v1 - orig.p[axis]) / dir.p[axis];

	// behind the plane
	//if (a < 0)
	//	return false;

	// other axis
	int axis2 = (axis + 1) % 3;
	int axis3 = (axis + 2) % 3;

	// other axis values
	float v2 = a * dir.p[axis2] + orig.p[axis2];
	float v3 = a * dir.p[axis3] + orig.p[axis3];

	// check 2D aabb
	if (pos.p[axis2] < v2 && v2 < pos.p[axis2] + 2 * radius)
	if (pos.p[axis3] < v3 && v3 < pos.p[axis3] + 2 * radius)
		return true;
	return false;
}

bool AABB::hit(const Vec3Df& orig, const Vec3Df& dir) {
	for (int i = 0; i < 3; i++)
		if (collidePlane(i, orig, dir))
			return true;
	return false;
}

inline float intersect(const Vec3Df& orig, const Vec3Df& dir, const Triangle* const triangle) {
	const Vertex* vertices = triangle->vertices;
	const Vec3Df& v0 = vertices[0].position;
	const Vec3Df& v1 = vertices[1].position;
	const Vec3Df& v2 = vertices[2].position;

	Vec3Df e1;
	e1.p[0] = v1.p[0] - v0.p[0];
	e1.p[1] = v1.p[1] - v0.p[1];
	e1.p[2] = v1.p[2] - v0.p[2];

	Vec3Df e2;
	e2.p[0] = v2.p[0] - v0.p[0];
	e2.p[1] = v2.p[1] - v0.p[1];
	e2.p[2] = v2.p[2] - v0.p[2];

	const Vec3Df P = cross(dir, e2);
	const float det = dot(e1, P);

	if (det < 0.000000001 && det > -0.000000001)
		return 0;

	float inv_det = 1.0f;
	inv_det /= det;

	Vec3Df T = orig;
	T -= vertices[0].position;

	float u = dot(T, P);
	u *= inv_det;
	if (u < 0.0f || u > 1.0f)
		return 0;

	const Vec3Df Q = cross(T, e1);

	float v = dot(dir, Q);
	v *= inv_det;
	if (v < 0.0f || u + v > 1.0f)
		return 0;

	float t = dot(e2, Q);
	t *= inv_det;

	return t;
}

float AABB::collide(const Vec3Df& orig, const Vec3Df& dir, Triangle** out) {
	// check hit with this cube
	if (!hit(orig, dir)) {
		*out = 0;
		return 1e10f;
	}

	// current closest triangle
	float shortest = 1e10f;
	Triangle* res = 0;

	// check with leaves
	for (unsigned int i = 0; i < leaves.size(); i++) {
		const float dist = intersect(orig, dir, leaves[i]);
		if (dist && dist < shortest) {
			shortest = dist;
			res = leaves[i];
		}
	}

	// check with subnodes
	// TODO: skip irrelevant subnodes
	if (sub) {
		for (int i = 0; i < 8; i++) {
			Triangle* res2;
			float dist = sub[i]->collide(orig, dir, &res2);
			if (dist < shortest) {
				res = res2;
				shortest = dist;
			}
		}
	}

	// hit
	*out = res;
	return shortest;
}

void Tree::calcSize(Mesh& mesh) {
	Vec3Df p1; // min
	Vec3Df p2; // max

	for (unsigned int i = 0; i < mesh.triangles.size(); i++) { // triangle
		for (int v = 0; v < 3; v++) { // vertex
			for (int d = 0; d < 3; d++) { // dimension
				const float dim = mesh.triangles[i].vertices[v].position.p[d];
				if (dim < p1.p[d])
					p1.p[d] = dim;
				if (dim > p2.p[d])
					p2.p[d] = dim;
			}
		}
	}
//	printf("min: (%.1f,%.1f,%.1f)\n", p1.p[X], p1.p[Y], p1.p[Z]);
//	printf("max: (%.1f,%.1f,%.1f)\n", p2.p[X], p2.p[Y], p2.p[Z]);

	Vec3Df dim = p2 - p1;
	float sdim = dim.p[X];
	sdim = fmax(sdim, dim.p[Y]);
	sdim = fmax(sdim, dim.p[Z]);

	root = new AABB(p1, sdim * 0.5f);
}

void Tree::build(Mesh& mesh) {
    max_depth = (int)(log(mesh.triangles.size()) / log(2));
    printf("building tree with size %d\n", max_depth);
	calcSize(mesh);

	for (unsigned int i = 0; i < mesh.triangles.size(); i++)
		add(mesh.triangles[i]);
}

void Tree::add(Triangle& tr) {
	AABB* current = root;
	int a0, a1, a2;

	unsigned int depth = 0;
	while (depth < max_depth) {
		a0 = current->follow(tr.vertices[0].position);
		a1 = current->follow(tr.vertices[1].position);
		a2 = current->follow(tr.vertices[2].position);

		if (a0 != a1 || a1 != a2)
			break;

		current->split();
		current = current->sub[a0];
		depth++;
	}

	current->leaves.push_back(&tr);
}

float Tree::collide(const Vec3Df& orig, const Vec3Df& dir, Triangle** out) {
	return root->collide(orig, dir, out);
}
