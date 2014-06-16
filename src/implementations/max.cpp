#include "max.hpp"
#include "../RayTracer.hpp"
#include "../Mesh.hpp"
#include "../Vec3D.hpp"
#include <cmath>

unsigned int raysTraced = 0;
bool debug = true;

Ray performRayTracingMax(Ray ray){
	raysTraced++;

	std::vector<Triangle> triangles = MyMesh.triangles;
	float hit = 10e6f;
	for (unsigned int i = 0; i < triangles.size(); i++){
		hit = fmin(hit, intersect(triangles[i], ray));
	}
	//hit = 1 / ((hit * 2) + 1); // Arithmetic function for getting a usable color.
	ray.setColor(Vec3Df(hit, hit/5, hit*5));
	return ray;
}

float intersect(Triangle t, Ray ray){
	Vec3Df v0 = t.vertices[0].p, // Variables saved as locals for less external function calls,
		v1 = t.vertices[1].p,    // less characters in the code and better readability.
		v2 = t.vertices[2].p,
		rayOrig = ray.getOrig(),

		triangleNormal = cross(v1 - v0, v2 - v0), // Normal of the triangle, and the plane the triangle lies on.
		rayDir = normal(ray.getDest() - rayOrig); // The normalized direction of the ray.
	float angle = dot(triangleNormal, rayDir); // The cosine of the angle of the vectors (dotproduct of the vectors).

	if (angle == 0) // If the ray and the plane are parallel (so their angle is 0), they won't intersect.
		return 10e6f;

	float rayD = -(dot(triangleNormal, rayOrig) + dot(triangleNormal, v0)) / angle; // The distance of the ray's origin
		// to the plane in which lies the triangle.

	Vec3Df rayHit = rayOrig + rayD * rayDir; // The intersection point of the ray and the plane in which lies the triangle.

	if (dot(triangleNormal, cross(v1 - v0, rayHit - v0)) < 0)
		return 10e6f;
	if (dot(triangleNormal, cross(v2 - v1, rayHit - v1)) < 0)
		return 10e6f;
	if (dot(triangleNormal, cross(v0 - v2, rayHit - v2)) < 0)
		return 10e6f;

	return rayD;
}

Vec3Df cross(Vec3Df in1, Vec3Df in2){
	return in1.crossProduct(in1, in2);
}

float dot(Vec3Df in1, Vec3Df in2){
	return in1.dotProduct(in1, in2);
}

Vec3Df normal(Vec3Df in){
	Vec3Df res = Vec3Df(in);
	res.normalize();
	return res;
}
