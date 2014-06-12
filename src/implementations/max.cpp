#include "max.hpp"
#include "../RayTracer.hpp"
#include "../Mesh.hpp"
#include "../Vec3D.hpp"

unsigned int raysTraced = 0;

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
	Vec3Df v0 = MyMesh.vertices[t.v[0]].p, v1 = MyMesh.vertices[t.v[1]].p, v2 = MyMesh.vertices[t.v[2]].p, rayOrig = ray.getOrig();
	
	Vec3Df triangleNormal = rayOrig.crossProduct(v1 - v0, v2 - v0);
	Vec3Df rayDir = Vec3Df(ray.getDest()[0] - rayOrig[0], ray.getDest()[1] - rayOrig[1], ray.getDest()[2] - rayOrig[2]);
	float dotTrNRDir = rayOrig.dotProduct(triangleNormal, rayDir);
	
	if (dotTrNRDir == 0){
		return false;
	}

	float rayD = -(rayOrig.dotProduct(triangleNormal, rayOrig) + rayOrig.dotProduct(triangleNormal, v0)) / dotTrNRDir;
	Vec3Df rayHit = rayOrig + rayD * rayDir;

	if (rayOrig.dotProduct(triangleNormal, rayOrig.crossProduct(v1 - v0, rayHit - v0)) < 0)
		return false;
	if (rayOrig.dotProduct(triangleNormal, rayOrig.crossProduct(v2 - v1, rayHit - v1)) < 0)
		return false;
	if (rayOrig.dotProduct(triangleNormal, rayOrig.crossProduct(v0 - v2, rayHit - v2)) < 0)
		return false;
	return true;
}