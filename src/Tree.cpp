#include "Tree.hpp"
#include <cstdio>

AABB::AABB() : sub(0), radius(-1) { }

AABB::AABB(POS pos, float radius) {
	COPY3(this->pos, pos);
	this->radius = radius;
	sub = 0;
}

AABB::~AABB() {
	// can safeley delete the null pointer
	delete[] sub;
}

int AABB::follow(VEC vec) {
	int axis = 0;
	for (int a = 0; a < 3; a++) {
		if (vec[a] > pos[a] + radius)
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
				SVEC diff, subpos;
				SETVEC(diff, x * radius, y * radius, z * radius);
				ADD(subpos, pos, diff);
				float subradius = radius * 0.5f;

				sub[z * 4 + y * 2 + x] = new AABB(subpos, subradius);
			}
		}
	}
}

bool AABB::collidePlane(RAY ray, int axis) {
	// check axis plane
	float v1 = pos[axis];

	// flip
	bool flip = (ray[axis + 3] < 0);
	//if (dir.p[axis] > 0 && orig.p[axis] > v1) flip = !flip;
	//if (dir.p[axis] < 0 && orig.p[axis] < v1 + 2 * radius) flip = !flip;
	v1 += flip * 2 * radius;

	// skip
	if ((v1 - ray[axis] - flip * 2 * radius) / ray[axis + 3] < 0)
		return false;

	// factor
	float a = (v1 - ray[axis]) / ray[axis + 3];

	// behind the plane
	//if (a < 0)
	//	return false;

	// other axis
	int axis2 = (axis + 1) % 3;
	int axis3 = (axis + 2) % 3;

	// other axis values
	float v2 = a * ray[axis2 + 3] + ray[axis2];
	float v3 = a * ray[axis3 + 3] + ray[axis3];

	// check 2D aabb
	if (pos[axis2] < v2 && v2 < pos[axis2] + 2 * radius)
	if (pos[axis3] < v3 && v3 < pos[axis3] + 2 * radius)
		return true;
	return false;
}

bool AABB::hit(RAY ray) {
	for (int i = 0; i < 3; i++)
		if (collidePlane(ray, i))
			return true;
	return false;
}

inline float intersect(RAY ray, POLY poly) {
	VEC v0 = poly + 0;
	VEC v1 = poly + 3;
	VEC v2 = poly + 6;

	SVEC e1, e2, P, T, Q;
	float det, u, v, t;

	SUB(e1, v1, v0);
	SUB(e2, v2, v0);
	
	CROSS(P, (ray+3), e2);
	DOT(det, e1, P);

	if (det < 0.000000001 && det > -0.000000001)
		return 0;

	float inv_det = 1.0f;
	inv_det /= det;

	SUB(T, ray, v0);

	DOT(u, T, P);

	u *= inv_det;

	if (u < 0.0f || u > 1.0f)
		return 0;

	CROSS(Q, T, e1);

	DOT(v, (ray+3), Q);
	v *= inv_det;
	if (v < 0.0f || u + v > 1.0f)
		return 0;

	DOT(t, e2, Q);
	t *= inv_det;

	return t;
}

float AABB::collide(RAY ray, OUT POLY* poly) {
	// check hit with this cube
	if (!hit(ray)) {
		*poly = 0;
		return 1e10f;
	}

	// current closest triangle
	float shortest = 1e10f;
	POLY res = 0;

	// check with leaves
	for (unsigned int i = 0; i < leaves.size(); i++) {
		const float dist = intersect(ray, leaves[i]);
		if (dist && dist < shortest) {
			shortest = dist;
			res = leaves[i];
		}
	}

	// check with subnodes
	// TODO: skip irrelevant subnodes
	if (sub) {
		for (int i = 0; i < 8; i++) {
			POLY res2;
			float dist = sub[i]->collide(ray, poly);
			if (dist < shortest) {
				res = res2;
				shortest = dist;
			}
		}
	}

	// hit
	*poly = res;
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

	Vec3Df dim = p2 - p1;
	float sdim = dim.x;
	sdim = fmax(sdim, dim.y);
	sdim = fmax(sdim, dim.z);

	root = new AABB(p1.p, sdim * 0.5f);
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

	SPOLY spoly;
	POLY p = spoly;
	COPY3((p + 0), tr.vertices[0].position.p);
	COPY3((p + 3), tr.vertices[1].position.p);
	COPY3((p + 6), tr.vertices[2].position.p);

	unsigned int depth = 0;
	while (depth < max_depth) {
		a0 = current->follow(tr.vertices[0].position.p);
		a1 = current->follow(tr.vertices[1].position.p);
		a2 = current->follow(tr.vertices[2].position.p);

		if (a0 != a1 || a1 != a2)
			break;

		current->split();
		current = current->sub[a0];
		depth++;
	}

	//current->leaves.push_back(spoly);
	//current->leaves.emplace_back({ 1, 2, 3, 4, 5, 6, 7, 8, 9 });
	float* dyn = new float[9];
	memcpy(dyn, spoly, 9 * sizeof(float));
	current->leaves.push_back(dyn);
}

float Tree::collide(RAY ray, OUT POLY* p) {
	return root->collide(ray, p);
}
