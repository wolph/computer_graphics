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
		else if (name == "sphere") {
			Vec3Df pos(x, y, z);
			float r;
			scene >> r;
			Sphere* sphere = new Sphere(pos, r);
			add(sphere);
		}
		else {
			Mesh* mesh = new Mesh(name, "mesh/" + name + ".obj");
			Vec3Df pos(x, y, z);
			add(new Object(pos, *mesh));
		}
		scene >> name >> x >> y >> z;
	}

    scene.close();
}

void Scene::add(Object* object) {
    objectIndex = (unsigned int)objects.size();
	objects.push_back(object);
    this->object = objects[objectIndex];
}
void drawCube(AABB* cube){
	if (cube->sub) {
		//glColor3f(1, 0.5, 0.5);
		for (int i = 0; i < 8; i++)
			drawCube(cube->sub[i]);
	}
	if (!cube->leaves.empty()) {
		//glColor3f(0, 0.5, 0.5);
		float dim = cube->radius * 2;
		for (int axis = 0; axis < 3; axis++){
			for (int x = 0; x < 2; x++){
				for (int y = 0; y < 2; y++){
					Vec3Df v = cube->pos;
					v.p[(axis + 1) % 3] += x * dim;
					v.p[(axis + 2) % 3] += y * dim;
					glVertex3f(v.p[X], v.p[Y], v.p[Z]);
					glVertex3f(v.p[X] + ((axis == 0) ? dim : 0),
						v.p[Y] + ((axis == 1) ? dim : 0),
						v.p[Z] + ((axis == 2) ? dim : 0));
				}
			}
		}
	}
}

void Scene::draw() {
	for (Object* obj : objects)
		obj->draw();

	// draw octree
	for (Object* obj : objects){
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glTranslatef(obj->pos.p[0], obj->pos.p[1], obj->pos.p[2]);
		glBegin(GL_LINES);
		drawCube(obj->tree.root);
		glEnd();
		glPopAttrib();
	}
}

void Scene::update() {
	cam.Update();
	for (Object* obj : objects)
		obj->pos += obj->vel;
}

Material defaultMat;

bool Scene::raytrace(const Vec3Df& orig, const Vec3Df& dir, Vec3Df* impact, Vec3Df* normal, Material** mat, Object** obj, Vec3Df* color) {
	float close = 1e10f;
	*normal = Vec3Df(0, 0, 0);
	*mat = &defaultMat;
	*obj = 0;

	for (unsigned int i = 0; i < objects.size(); i++) {
		Vec3Df tempNormal;
		Vec3Df tempImpact;
		Material* tempMat;
		float dist = objects[i]->raytrace(orig, dir, &tempImpact, &tempNormal, &tempMat, color);
		if (dist < close) {
			*normal = tempNormal;
			*impact = tempImpact;
			*mat = tempMat;
			*obj = objects[i];
			close = dist;
		}
	}
	if (close == 1e10f)
		return false;
	return true;
}

void Scene::addLightPoint(Vec3Df& lightPos) {
	this->lights.push_back(lightPos);
}

void drawNormal(const Vec3Df& avg, const Vec3Df& n){
	glVertex3f(avg.p[X], avg.p[Y], avg.p[Z]);
	Vec3Df d = avg + n * 0.1f;
	glVertex3f(d.p[X], d.p[Y], d.p[Z]);
}

void drawNormals(Object* obj) {
	for (unsigned int i = 0; i < obj->mesh.triangles.size(); i++){
		Vec3Df avg;
		for (int t = 0; t < 3; t++)
			avg += obj->mesh.triangles[i].vertices[t].position * 0.333f;

		drawNormal(avg + obj->pos, obj->mesh.triangles[i].normal);
	}

	for (unsigned int i = 0; i < obj->mesh.vertices.size(); i++)
		drawNormal(obj->mesh.vertices[i].position + obj->pos, obj->mesh.vertices[i].normal);
}

void Scene::debugDraw() {
    glColor3f(1, 0.5, 0.5);
    glLineWidth(3);

    //glBegin(GL_LINES);
    //for (Object* obj : objects)
    //    // draw octree
    //    drawCube(obj->tree.root);
    //// draw normals
    ////			drawNormals(obj);
    //glEnd();
}

void Object::draw() {
	glPushMatrix();
	glTranslatef(pos.p[X], pos.p[Y], pos.p[Z]);
	mesh.drawSmooth();
	glPopMatrix();
}

void Sphere::draw() {
	glPushMatrix();
	glTranslatef(center.p[X], center.p[Y], center.p[Z]);
	glutSolidSphere(radius, 20, 20);
	glPopMatrix();
}

//float Object::raytrace(const Ray& ray, Triangle** tr) {
//	Ray disp(ray.orig + pos, ray.dest + pos);
//	return tree.collide(disp, tr);
//}

// surface of triangle
// heron formula
float surface(float* t){

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

// incredibly beautiful convenience function
float surface(const Vec3Df& a, const Vec3Df& b, const Vec3Df& c){
	float f[9];
	*((Vec3Df*)&f[0]) = a;
	*((Vec3Df*)&f[3]) = b;
	*((Vec3Df*)&f[6]) = c;
	return surface(f);
}

float Object::raytrace(const Vec3Df& orig, const Vec3Df& dir, Vec3Df* impact, Vec3Df* normal, Material** mat, Vec3Df* color) {
	Triangle* tr;
	float dist = tree.collide(orig - pos, dir, &tr);
	*impact = orig + dist * dir;

	if (!tr)
		return 1e10f;

	// calc areas
	float a1 = surface(tr->vertices[1].position, *impact - pos,
		tr->vertices[2].position);
	float a2 = surface(tr->vertices[0].position, *impact - pos,
		tr->vertices[2].position);
	float a3 = surface(tr->vertices[0].position, *impact - pos,
		tr->vertices[1].position);
	float total = a1 + a2 + a3;

	// factors
	float f1 = a1 / total;
	float f2 = a2 / total;
	float f3 = a3 / total;

	// calc normal
	*normal = 
	    f1 * tr->vertices[0].normal 
	    + f2 * tr->vertices[1].normal
		+ f3 * tr->vertices[2].normal;
	normal->normalize();

	*mat = (Material*) &tr->material;

//    *color

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
    return 1e10f;
}

float Sphere::raytrace(const Vec3Df& orig, const Vec3Df& dir, Vec3Df* impact, Vec3Df* normal, Material** mat, Vec3Df* color) {
	//float dist = intersectSphere(orig, dir, center, radius, impact);
	float dist = intersect(orig, dir, center + MyScene.cam.pos, radius, impact);

	*mat = &defaultMat;
	*normal = *impact - center;
	//float d = impact->getLength();
	//printf("%f\n", d);
	normal->normalize();
	return dist;
}

Object* Scene::nextObject(){
    objectIndex = (unsigned int)(objectIndex ? objectIndex : objects.size()) - 1;
    return object = objects[objectIndex];
}

Object* Scene::prevObject(){
    objectIndex = (unsigned int)(objectIndex ? objectIndex : objects.size()) - 1;
    return object = objects[objectIndex];
}
