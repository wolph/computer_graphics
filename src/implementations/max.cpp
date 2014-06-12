#include "max.h"
#include "../RayTracer.h"
#include "../Mesh.h"

Ray performRayTracingMax(Ray ray){
	
	std::vector<Triangle> triangles = MyMesh.triangles;
	ray.setColor(Vec3Df(0, 0, 0));
	//float resColor = 1000.0f;
	bool hit = false;
	for (unsigned int i = 0; i < triangles.size(); i++){
		hit = hit || intersect(triangles[i], ray);
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

Vec3Df normalize(Vec3Df in){
	float length = sqrt(in[0]*in[0] + in[1]*in[1] + in[2]*in[2]);
	return Vec3Df(in[0] / length, in[1] / length, in[2] / length);
}

bool intersect(Triangle t, Ray ray){
	Vec3Df v0 = MyMesh.vertices[t.v[1]].p, v1 = MyMesh.vertices[t.v[1]].p, v2 = MyMesh.vertices[t.v[2]].p;
	
	Vec3Df rayOrig = ray.getOrig();
	Vec3Df triangleNormal = crossProduct(v1 - v0, v2 - v0);
	Vec3Df rayDir = normalize(Vec3Df(ray.getDest()[0] - rayOrig[0], ray.getDest()[1] - rayOrig[1], ray.getDest()[2] - rayOrig[2]));
	
	if (dotProduct(triangleNormal, rayDir) == 0)
		return false;
	float origD = dotProduct(triangleNormal, v0);
	float rayD = -(dotProduct(triangleNormal, rayOrig) + origD) / dotProduct(triangleNormal, rayDir);
	Vec3Df rayHit = rayOrig + rayD * rayDir;

	if (dotProduct(triangleNormal, crossProduct(v1 - v0, rayHit - v0)) < 0)
		return false;
	if (dotProduct(triangleNormal, crossProduct(v2 - v1, rayHit - v1)) < 0)
		return false;
	if (dotProduct(triangleNormal, crossProduct(v0 - v2, rayHit - v2)) < 0)
		return false;
	return true;
}