#include "max.hpp"
#include "../RayTracer.hpp"
#include "../Mesh.hpp"
#include "../Vec3D.hpp"
#include <cmath>

void performRayTracingMax(Ray& ray){
	float hit = 10e6f;
	unsigned int amountOfTriangles = MyMesh.triangles.size();
	for (unsigned int i = 0; i < amountOfTriangles; i++){
		float ins = intersect2(MyMesh.triangles[i], ray);
		if (ins < hit)
			hit = ins;
	}
	//hit = 1 / ((hit * 2) + 1); // Arithmetic function for getting a usable color.
	ray.setColor(Vec3Df(hit, hit/5, hit*5));
}

#define dot Vec3Df::dotProduct
#define cross Vec3Df::crossProduct

float intersect2(const Triangle& t, const Ray& ray){
	const Vec3Df& v0 = t.vertices[0].p, // Variables saved as locals for less external function calls,
		v1 = t.vertices[1].p,    // less characters in the code and better readability.
		v2 = t.vertices[2].p;
	float angle = dot(t.normal, ray.dir); // The cosine of the angle of the vectors (dotproduct of the vectors).

    /* Floats are only rarely exactly 0, are you sure this is correct? */
	if (angle == 0) // If the ray and the plane are parallel (so their angle is 0), they won't intersect.
		return 10e6f;

	float rayD = -(dot(t.normal, ray.orig) + dot(t.normal, v0)) / angle; // The distance of the ray's origin
		// to the plane in which lies the triangle.

	Vec3Df rayHit = ray.orig + rayD * ray.dir; // The intersection point of the ray and the plane in which lies the triangle.

	if (dot(t.normal, cross(v1 - v0, rayHit - v0)) < 0)
		return 10e6f;
	if (dot(t.normal, cross(v2 - v1, rayHit - v1)) < 0)
		return 10e6f;
	if (dot(t.normal, cross(v0 - v2, rayHit - v2)) < 0)
		return 10e6f;

	return rayD;
}
