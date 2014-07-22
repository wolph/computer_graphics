#include "Scene.hpp"
#include <cstdio>
#include <fstream>
#include "Material.hpp"
#include "Vec3D.hpp"

extern Scene MyScene;

void Scene::load(string path) {
	floorheight = 0;

	ifstream scene(path);
	string name;
	float x, y, z;

	scene >> name >> x >> y >> z;
	while(scene) {
		if (name == "light") {
			Vec3Df lightPoint(x, y, z);
			addLightPoint(lightPoint);
		}
		else if (name == "camera") {
			cam.pos.p[X] = x;
			cam.pos.p[Y] = y;
			cam.pos.p[Z] = z;
			float xrot, yrot;
			scene >> xrot >> yrot;
			cam.xrot = xrot;
			cam.yrot = yrot;
		}
		else if (name == "floor") {
			floorheight = y;
		}
		else {
			Mesh* mesh = new Mesh(name, "mesh/" + name + ".obj");
			Vec3Df pos(x, y, z);
			add(new Object(pos, *mesh));
		}
		scene >> name >> x >> y >> z;
	}

    scene.close();

	printf("Done loading scene!\n");
}

void Scene::add(Object* object) {
    objectIndex = (unsigned int)objects.size();
	objects.push_back(object);
    this->object = objects[objectIndex];
}

void Scene::draw() {

}

void Scene::debugDraw() {

}

void Scene::update() {
	cam.Update();
	for (Object* obj : objects)
		obj->pos += obj->vel;
}

Material defaultMat;

bool Scene::raytrace(RAY ray, OUT POS impact, OUT VEC normal) {
	float close = 1e10f;

	for (unsigned int i = 0; i < objects.size(); i++) {
		Vec3Df tempNormal;
		Vec3Df tempImpact;
		Material* tempMat;
		float dist = objects[i]->raytrace(ray, impact, normal);
		close = dist;
		/*if (dist < close) {
			*normal = tempNormal;
			*impact = tempImpact;
			*mat = tempMat;
			*obj = objects[i];
			close = dist;
		}*/
	}
	if (close == 1e10f)
		return false;
	return true;
}

void Scene::addLightPoint(Vec3Df& lightPos) {
	this->lights.push_back(lightPos);
}

// surface of triangle
// heron formula
float surface(POLY t) {

	// side lengths
	float a = sqrt(
		(t[0] - t[3]) * (t[0] - t[3]) + (t[1] - t[4]) * (t[1] - t[4])
		+ (t[2] - t[5]) * (t[2] - t[5]));
	float b = sqrt(
		(t[0] - t[6]) * (t[0] - t[6]) + (t[1] - t[7]) * (t[1] - t[7])
		+ (t[2] - t[8]) * (t[2] - t[8]));
	float c = sqrt(
		(t[3] - t[6]) * (t[3] - t[6]) + (t[4] - t[7]) * (t[4] - t[7])
		+ (t[5] - t[8]) * (t[5] - t[8]));

	// om
	float s = 0.5f * (a + b + c);

	// area
	float area = sqrt(s * (s - a) * (s - b) * (s - c));
	return area;
}

// surface of triangle
// heron formula
float surface(VEC t1, VEC t2, VEC t3) {

	// side lengths
	float a = sqrt(
		(t1[0] - t2[3]) * (t1[0] - t2[3-3]) + (t1[1] - t2[4-3]) * (t1[1] - t2[4])
		+ (t1[2] - t2[5]) * (t1[2] - t2[5-3]));
	float b = sqrt(
		(t1[0] - t3[6]) * (t1[0] - t3[6-6]) + (t1[1] - t3[7-6]) * (t1[1] - t3[7-6])
		+ (t1[2] - t3[8-6]) * (t1[2] - t3[8-6]));
	float c = sqrt(
		(t2[3-3] - t3[6-6]) * (t2[3-3] - t3[6-6]) + (t2[4-3] - t3[7-6]) * (t2[4-3] - t3[7-6])
		+ (t2[5-3] - t3[8-6]) * (t2[5-3] - t3[8-6]));

	// om
	float s = 0.5f * (a + b + c);

	// area
	float area = sqrt(s * (s - a) * (s - b) * (s - c));
	return area;
}

float Object::raytrace(RAY ray, OUT POS impact, OUT VEC normal) {

	SRAY ray2;
	SUB(ray2, ray, pos);
	COPY3(ray2 + 3, ray + 3);
	POLY poly;
	float dist = tree.collide(ray2, &poly);

	// calculate impact
	// *impact = orig + dist * dir;
	SVEC product;
	COPY3(product, ray + 3);
	MULS(product, dist);
	ADD(impact, ray, product);

	if (dist == 1e10f)
		return 1e10f;

	// calc areas
	SVEC temp;
	SUB(temp, impact, ray);
	float a1 = surface(poly + 1 * 3, temp, poly + 2 * 3);
	float a2 = surface(poly + 0 * 3, temp, poly + 2 * 3);
	float a3 = surface(poly + 0 * 3, temp, poly + 2 * 3);
	float total = a1 + a2 + a3;

	// factors
	float f1 = a1 / total;
	float f2 = a2 / total;
	float f3 = a3 / total;

	// calc normal
	//*normal = 
	//    f1 * tr->vertices[0].normal 
	//    + f2 * tr->vertices[1].normal
	//	+ f3 * tr->vertices[2].normal;
	//normal->normalize();

	SETCOLOR(normal, 1, 0, 0);

	return dist;
}

float intersect(const Vec3Df& p, const Vec3Df& d, const Vec3Df& c, float r, Vec3Df* ins) {
	Vec3Df vpc = c - p;  // this is the vector from p to c

	if (dot(vpc, d) < 0) {
		// when the sphere is behind the origin p
		// note that this case may be dismissed if it is 
		// considered that p is outside the sphere 	
		if (vpc.getSquaredLength() > r * r) {
			// there is no intersection
			return 1e10f;
		}
		else if (vpc.getSquaredLength() == r * r) {
			// found the intersection
			*ins = p;
		}
		else {
			// occurs when p is inside the sphere
			//pc = projection of c on the line;
			Vec3Df pc = dot(c, d) * d;
			// distance from pc to i1
			float dist = sqrt(r*r - (pc-c).getSquaredLength());
			float di1 = dist - (pc-p).getLength();
			*ins = p + d * di1;
			return dist;
		}

	}
	else {
		// center of sphere projects on the ray
		// pc = projection of c on the line
		Vec3Df pc = dot(c, d) * d;
		if ((c - pc).getSquaredLength() > r*r) {
			// there is no intersection
			return 1e10f;
		}
		else {
			// distance from pc to i1
			float dist = sqrt(r*r - (pc - c).getSquaredLength());
			float di1;
			if (vpc.getSquaredLength() > r * r) {
				// origin is outside sphere	
				di1 = (pc - p).getLength() - dist;
			}
			else {// origin is inside sphere
				di1 = (pc - p).getLength() + dist;
			}
			*ins = p + d * di1;
			return dist;
		}
	}
}
