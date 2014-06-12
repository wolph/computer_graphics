#include "max.h"
#include "../RayTracer.h"
#include "../Mesh.h"

Ray performRayTracingMax(Ray ray){
	
	std::vector<Triangle> triangles = MyMesh.triangles;
	ray.setColor(Vec3Df(0, 0, 0));
	//float resColor = 1000.0f;
	bool hit = false;
	for (unsigned int i = 0; i < triangles.size(); i++){
		hit |= intersect(triangles[i], ray);
	}
	if (hit)
		ray.setColor(Vec3Df(0, 1, 0));
	return ray;
}

Vec3Df crossProduct(Vec3Df in1, Vec3Df in2){
	return Vec3Df(in1[2] * in2[3] - in1[3] * in2[2],
		in1[3] * in2[1] - in1[1] * in2[3],
		in1[1] * in2[2] - in1[2] * in2[1]);
}

float dotProduct(Vec3Df in1, Vec3Df in2){
	return (in1[0] * in2[0] + in1[1] * in2[1] + in1[2] * in2[2]);
}

bool intersect(Triangle t, Ray ray){
	return true;
	std::vector<Vertex> vertices = MyMesh.vertices;
	Vec3Df v0 = vertices[t.v[1]].p, v1 = vertices[t.v[1]].p, v2 = vertices[t.v[2]].p;
	Vec3Df triangleNormal = crossProduct(v1 - v0, v2 - v0);
	if (dotProduct(triangleNormal, ray.getDest()) == 0)
		return false;
	float origD = dotProduct(triangleNormal, vertices[t.v[0]].p);
	float rayD = -(dotProduct(triangleNormal, ray.getOrig()) + origD) / dotProduct(triangleNormal, ray.getDest());
	Vec3Df rayHit = ray.getOrig() + rayD * ray.getDest();

	if (dotProduct(triangleNormal, crossProduct(v1 - v0, rayHit - v0)) < 0)
		return false;
	if (dotProduct(triangleNormal, crossProduct(v2 - v1, rayHit - v1)) < 0)
		return false;
	if (dotProduct(triangleNormal, crossProduct(v0 - v2, rayHit - v2)) < 0)
		return false;
	return true;
}