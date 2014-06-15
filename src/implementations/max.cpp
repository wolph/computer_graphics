#include "max.hpp"
#include "../RayTracer.hpp"
#include "../Mesh.hpp"
#include "../Vec3D.hpp"

unsigned int raysTraced = 0;
bool debug = false;

Ray performRayTracingMax(Ray ray){
	raysTraced++;
	
	std::vector<Triangle> triangles = MyMesh.triangles;
	ray.setColor(Vec3Df(0, 0, 0));
	bool hit = false;
	for (unsigned int i = 0; i < triangles.size(); i++){
		hit = hit || intersect(triangles[i], ray);
	}
	if (hit)
		ray.setColor(Vec3Df(0, 1, 0));
	return ray;
}

bool intersect(Triangle t, Ray ray){
	Vec3Df v0 = MyMesh.vertices[t.v[0]].p, // Variables saved to locals for less external function calls,
		v1 = MyMesh.vertices[t.v[1]].p,    // less characters in the code and better readability.
		v2 = MyMesh.vertices[t.v[2]].p,
		rayOrig = ray.getOrig(),
	
		v1mv0 = v1 - v0, // Saved as variable for (minimal) performace improvement, result is used twice.
		triangleNormal = cross(v1mv0, v2 - v0), // Normal of the triangle, and the plane the triangle lies on.
		rayDir = normal(ray.getDest() - rayOrig); // The normalized direction of the ray.
	float angle = dot(triangleNormal, rayDir); // The cosine of the angle of the vectors (dotproduct of the vectors).
	
	int res[5] = { 0, 0, 0, 0, 0 };

	if (angle == 0) // If the ray and the plane are parallel (so their angle is 0), they won't intersect.
		res[0] = 1;

	float rayD = -(dot(triangleNormal, rayOrig) + dot(triangleNormal, v0)) / angle; // The distance of the ray's origin
		// to the plane in which lies the triangle.

	if (rayD < 0) // If the intersection point of the ray and the triangle is behind the ray's origin, return false.
		res[1] = 1;

	Vec3Df rayHit = rayOrig + rayD * rayDir; // The point where the ray will hit the plane in which lies the triangle.

	if (dot(triangleNormal, cross(v1mv0, rayHit - v0)) < 0)
		res[2] = 1;
	if (dot(triangleNormal, cross(v2 - v1, rayHit - v1)) < 0)
		res[3] = 1;
	if (dot(triangleNormal, cross(v0 - v2, rayHit - v2)) < 0)
		res[4] = 1;
	
	if (debug)
		std::cout << res[0] << " " << res[1] << " " << res[2] << " " << res[3] << " " << res[4] << std::endl;

	if (res[0] + res[1] + res[2] + res[3] + res[4] > 0)
		return false;
	return true;
}

Vec3Df cross(Vec3Df in1, Vec3Df in2){
	return in1.crossProduct(in1, in2);
}
float dot(Vec3Df in1, Vec3Df in2){
	return in1.dotProduct(in1, in2);
}

Vec3Df normal(Vec3Df in){
	Vec3Df res = Vec3Df(in);
	res /= res.normalize();
	return res;
}