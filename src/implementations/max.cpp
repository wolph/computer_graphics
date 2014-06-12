#include "max.h"
#include "../RayTracer.h"
#include "../Mesh.h"

Ray performRayTracingMax(Ray ray){
	std::vector<Vertex> vertices = MyMesh.vertices;
	std::vector<Triangle> triangles = MyMesh.triangles;
	ray.setColor(Vec3Df(0, 0, 0));
	float resColor = 1000.0f;
	for (unsigned int i = 0; i < triangles.size(); i++){
		Triangle curT = triangles[i];
		Vec3Df triangleNormal = crossProduct(minus(vertices[curT.v[1]].p, vertices[curT.v[0]].p), minus(vertices[curT.v[2]].p, vertices[curT.v[0]].p));
		float planeD = dotProduct(triangleNormal, vertices[curT.v[0]].p);
		float rayD = -(dotProduct(triangleNormal, ray.getOrig()) + planeD ) / dotProduct(triangleNormal, ray.getDest());
		if (rayD < resColor)
			resColor = rayD;
	}
	ray.setColor(Vec3Df(resColor-50, resColor, resColor-10));
	std::cout << resColor << std::endl;
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

inline Vec3Df minus(Vec3Df in1, Vec3Df in2){
	return Vec3Df(in1[0] - in2[0], in1[1] - in2[1], in1[2] - in2[2]);
}
