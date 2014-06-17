#include "ymte.hpp"
#include "../Mesh.hpp"
#include "../Vec3D.hpp"
#include <cmath>

extern Mesh MyMesh;

float intersect2(Triangle& t, Ray& ray);

void performRayTracingYmte(Ray& ray) {
	float hit = 10e6f;
	unsigned int amountOfTriangles = MyMesh.triangles.size();
	for (unsigned int i = 0; i < amountOfTriangles; i++){
		hit = fmin(hit, intersect2(MyMesh.triangles[i], ray));
	}
	//hit = 1 / ((hit * 2) + 1); // Arithmetic function for getting a usable color.
	ray.setColor(Vec3Df(hit, hit / 5, hit * 5));
}

inline float intersect2(Triangle& t, Ray& ray){
	Vec3Df v0, v1, v2;
	v0 = t.vertices[0].p; // Variables saved as locals for less external function calls,
	v1 = t.vertices[1].p;    // less characters in the code and better readability.
	v2 = t.vertices[2].p;

	float angle = Vec3Df::dotProduct(t.normal, ray.dir); // The cosine of the angle of the vectors (dotproduct of the vectors).

	if (angle == 0) // If the ray and the plane are parallel (so their angle is 0), they won't intersect.
		return 10e6f;

	float rayD = -(Vec3Df::dotProduct(t.normal, ray.orig) + Vec3Df::dotProduct(t.normal, v0)) / angle; // The distance of the ray's origin
	// to the plane in which lies the triangle.

	Vec3Df rayHit = ray.orig + rayD * ray.dir; // The intersection point of the ray and the plane in which lies the triangle.

	if (Vec3Df::dotProduct(t.normal, Vec3Df::crossProduct(v1 - v0, rayHit - v0)) < 0)
		return 10e6f;
	if (Vec3Df::dotProduct(t.normal, Vec3Df::crossProduct(v2 - v1, rayHit - v1)) < 0)
		return 10e6f;
	if (Vec3Df::dotProduct(t.normal, Vec3Df::crossProduct(v0 - v2, rayHit - v2)) < 0)
		return 10e6f;

	return rayD;
}
