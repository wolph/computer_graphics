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

inline Vec3Df minus_(Vec3Df in1, Vec3Df in2){
	return Vec3Df(in1[0] - in2[0], in1[1] - in2[1], in1[2] - in2[2]);
}

bool intersect(Triangle t, Ray ray){
	std::vector<Vertex> vertices = MyMesh.vertices;
	Vec3Df triangleNormal = crossProduct(minus_(vertices[t.v[1]].p, vertices[t.v[0]].p), minus_(vertices[t.v[2]].p, vertices[t.v[0]].p));
	if (dotProduct(triangleNormal, ray.getDest()) == 0)
		return false;
	float origD = dotProduct(triangleNormal, vertices[t.v[0]].p);
	float rayD = -(dotProduct(triangleNormal, ray.getOrig()) + origD) / dotProduct(triangleNormal, ray.getDest());
	Vec3Df rayHit = ray.getOrig() + rayD * ray.getDest();

	if (dotProduct(triangleNormal, crossProduct(minus_(vertices[t.v[1]].p, vertices[t.v[0]].p), minus_(rayHit, vertices[t.v[0]].p))) < 0)
		return false;
	if (dotProduct(triangleNormal, crossProduct(minus_(vertices[t.v[2]].p, vertices[t.v[1]].p), minus_(rayHit, vertices[t.v[1]].p))) < 0)
		return false;
	if (dotProduct(triangleNormal, crossProduct(minus_(vertices[t.v[0]].p, vertices[t.v[2]].p), minus_(rayHit, vertices[t.v[2]].p))) < 0)
		return false;
	return true;
}