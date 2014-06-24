#include "Scene.hpp"
#include <cstdio>
#include <fstream>
#include "Material.hpp"
#include "Vec3D.hpp"

enum {
	MAIN = 0x4D4D, // Main Chunk
	VERSION = 0x0002, // M3D Version
	EDITOR = 0x3D3D, // 3D Editor Chunk
	OBJECT = 0x4000, // Object Block
	MESH = 0x4100, // Triangular Mesh
	VERTICES = 0x4110, // Vertices List
	FACES = 0x4120, // Faces Description
	FACEMAT = 0x4130, // Faces Material
	SMOOTH = 0x4150, // Smoothing Group List
	COORDMAP = 0x4140, // Mapping Coordinates List
	LOCALCOORD = 0x4160, // Local Coordinates System
	LIGHT = 0x4600, // Light
	SPOTLIGHT = 0x4610, // Spotlight
	CAM = 0x4700, // Camera
	MAT = 0xAFFF, // Material Block
	MATNAME = 0xA000, // Material Name
	ACOLOR = 0xA010, // Ambient Color
	DCOLOR = 0xA020, // Diffuse Color
	SCOLOR = 0xA030, // Specular Color
	TEXMAP1 = 0xA200, // Texture Map 1
	BUMPMAP = 0xA230, // Bump Map
	REFLMAP = 0xA220, // Reflection Map
	/* Sub Chunks For Each Map */
	MAPFILE = 0xA300, // Mapping Filename
	MAPPARAM = 0xA351, // Mapping Parameters
	KEYFRAMER = 0xB000, // Keyframer Chunk
	MESHINFO = 0xB002, // Mesh Information Block
	SPOTLIGHTINFO = 0xB007, // Spot Light Information Block
	FRAMES = 0xB008, // Frames (Start and End)
	NAME = 0xB010, // Object Name
	OBJPIVOT = 0xB013, // Object Pivot Point
	POSTRACK = 0xB020, // Position Track
	ROTTRACK = 0xB021, // Rotation Track
	SCALETRACK = 0xB022, // Scale Track
	HIERARCHYPOS = 0xB030, // Hierarchy Position
};

const char* idstrs[] = {
	"<unknown>", // Unknown Chunk ID
	"MAIN", // 0x4D4D: Main Chunk
	"VERSION", // 0x0002: M3D Version
	"EDITOR", // 0x3D3D: 3D Editor Chunk
	"OBJECT", // 0x4000: Object Block
	"MESH", // 0x4100: Triangular Mesh
	"VERTICES", // 0x4110: Vertices List
	"FACES", // 0x4120: Faces Description
	"FACEMAT", // 0x4130: Faces Material
	"SMOOTH", // 0x4150: Smoothing Group List
	"COORDMAP", // 0x4140: Mapping Coordinates List
	"LOCALCOORD", // 0x4160: Local Coordinates System
	"LIGHT", // 0x4600: Light
	"SPOTLIGHT", // 0x4610: Spotlight
	"CAM", // 0x4700: Camera
	"MAT", // 0xAFFF: Material Block
	"MATNAME", // 0xA000: Material Name
	"ACOLOR", // 0xA010: Ambient Color
	"DCOLOR", // 0xA020: Diffuse Color
	"SCOLOR", // 0xA030: Specular Color
	"TEXMAP1", // 0xA200: Texture Map 1
	"BUMPMAP", // 0xA230: Bump Map
	"REFLMAP", // 0xA220: Reflection Map
	/* Sub Chunks For Each Map */
	"MAPFILE", // 0xA300: Mapping Filename
	"MAPPARAM", // 0xA351: Mapping Parameters
	"KEYFRAMER", // 0xB000: Keyframer Chunk
	"MESHINFO", // 0xB002: Mesh Information Block
	"SPOTLIGHTINFO", // 0xB007: Spot Light Information Block
	"FRAMES", // 0xB008: Frames (Start and End)
	"NAME", // 0xB010: Object Name
	"OBJPIVOT", // 0xB013: Object Pivot Point
	"POSTRACK", // 0xB020: Position Track
	"ROTTRACK", // 0xB021: Rotation Track
	"SCALETRACK", // 0xB022: Scale Track
	"HIERARCHYPOS", // 0xB030: Hierarchy Position
};

void Model::load(std::string file) {
	printf("Loading model!\n");
	FILE* fp = fopen("mesh/temp/traintank.3ds", "rb");
	if (!fp) {
		printf("hoi\n");
		return;
	}

	for (int i = 0; i < 1e5; i++) {
	//while (!feof(fp)) {
		unsigned short id;
		unsigned int len;
		fread(&id, 1, 2, fp);
		fread(&len, 1, 4, fp);

		// handle chunk
		char name[21];
		name[20] = 0;
		char ch;
		int j = 0;
		unsigned short qty;
		Vec3Df vec;

		switch (id) {
		case MAIN:
		case EDITOR:
			break;
		case OBJECT:
			do {
				fread(&ch, 1, 1, fp);	
				name[j] = ch;
				j++;
			} while (ch && j < 20);
			printf("name = %s\n", name);
			break;
		case MESH:
			break;
		case VERTICES:
			fread(&qty, 2, 1, fp);
			printf("qtye = %d\n", qty);
			for (int i = 0; i < qty; i++) {
				Vec3Df vec;
				fread(vec.p, sizeof(vec), 1, fp);
				vertices.push_back(vec);
				//printf("(%f,%f,%f)\n", vec.p[0], vec.p[1], vec.p[2]);
			}
			break;
		case FACES:
			fread(&qty, sizeof(unsigned short), 1, fp);
			printf("number of polygons = %d\n", qty);
			for (int i = 0; i < qty; i++) {
				PolyIdx p;
				unsigned short flags;
				fread(&p, sizeof(p), 1, fp);
				fread(&flags, sizeof(unsigned short), 1, fp);
				polygons.push_back(p);
			}
			break;
		case COORDMAP:
			fread(&qty, sizeof(unsigned short), 1, fp);
			for (int i = 0; i < qty; i++) {
				float u, v;
				fread(&u, sizeof(float), 1, fp);
				fread(&v, sizeof(float), 1, fp);
			}
			break;
		default:
			//printf("unknown chunk id: %d\n", id);
			//printf("size = %d\n", len);
			fseek(fp, len - 6, SEEK_CUR);
			break;
		}


		//fseek(fp, len, SEEK_CUR);
	}

	fclose(fp);
}

void Scene::load(string path) {

	ifstream scene(path);
	string name;
	float x, y, z;

	scene >> name >> x >> y >> z;
	while(scene) {
		if (name == "light") {
			Vec3Df lightPoint(x, y, z);
			addLightPoint(lightPoint);
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
}

void Scene::add(Object* object) {
    objectIndex = (unsigned int)objects.size();
	objects.push_back(object);
    this->object = objects[objectIndex];
}

void Scene::draw() {
	for (Object* obj : objects)
		obj->draw();
}

void Scene::update() {
	for (Object* obj : objects)
		obj->pos += obj->vel;
}

Material defaultMat;

bool Scene::raytrace(const Vec3Df& orig, const Vec3Df& dir, Vec3Df* impact, Vec3Df* normal, Material** mat, Object** obj) {
	float close = 1e10f;
	*normal = Vec3Df(0, 0, 1);
	*mat = &defaultMat;
	*obj = 0;

	for (unsigned int i = 0; i < objects.size(); i++) {
		Vec3Df tempNormal;
		Vec3Df tempImpact;
		Material* tempMat;
		float dist = objects[i]->raytrace(orig, dir, &tempImpact, &tempNormal, &tempMat);
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
	glVertex3f(avg.p[0], avg.p[1], avg.p[2]);
	Vec3Df d = avg + n * 0.1f;
	glVertex3f(d.p[0], d.p[1], d.p[2]);
}

void drawNormals(Object* obj) {
	for (unsigned int i = 0; i < obj->mesh.triangles.size(); i++){
		Vec3Df avg;
		for (int t = 0; t < 3; t++)
			avg += obj->mesh.triangles[i].vertices[t].p * 0.333f;

		drawNormal(avg + obj->pos, obj->mesh.triangles[i].normal);
	}

	for (unsigned int i = 0; i < obj->mesh.vertices.size(); i++)
		drawNormal(obj->mesh.vertices[i].p + obj->pos, obj->mesh.vertices[i].n);
}

void Scene::debugDraw() {
		glColor3f(1, 0.5, 0.5);
		glLineWidth(3);
		glBegin(GL_LINES);
		//drawCube(MyTree.root);
		for (Object* obj : objects)
			drawNormals(obj);
		glEnd();
}

void Object::draw() {
	glPushMatrix();
	glTranslatef(pos.p[0], pos.p[1], pos.p[2]);
	mesh.drawSmooth();
	glPopMatrix();
}

void Sphere::draw() {
	glPushMatrix();
	glTranslatef(center.p[0], center.p[1], center.p[2]);
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

float Object::raytrace(const Vec3Df& orig, const Vec3Df& dir, Vec3Df* impact, Vec3Df* normal, Material** mat) {
	Triangle* tr;
	float dist = tree.collide(orig - pos, dir, &tr);
	*impact = orig + dist * dir;

	if (!tr)
		return 1e10f;

	// calc areas
	float a1 = surface(tr->vertices[1].p, *impact + pos,
		tr->vertices[2].p);
	float a2 = surface(tr->vertices[0].p, *impact + pos,
		tr->vertices[2].p);
	float a3 = surface(tr->vertices[0].p, *impact + pos,
		tr->vertices[1].p);
	float total = a1 + a2 + a3;

	// factors
	float f1 = a1 / total;
	float f2 = a2 / total;
	float f3 = a3 / total;

	// calc normal
	*normal = f1 * tr->vertices[0].n + f2 * tr->vertices[1].n
		+ f3 * tr->vertices[2].n;

	*mat = (Material*) &tr->material;
	return dist;
}


inline float intersectSphere(const Vec3Df& orig, const Vec3Df& dir, const Vec3Df& center, float r) {
	
	//float a = dot(dir, dir), b = 2 * dot(dir, orig),c = dot(orig, orig) - (r * r);
	Vec3Df L = (center - orig);
	float a = Vec3Df::dotProduct(dir, dir);
	float b = 2 * Vec3Df::dotProduct(dir, L);
	float c = Vec3Df::dotProduct(L, L) - r*r;
	// ABC formula
	float discr = ( b * b ) - ( 4 * a * c );
	if (discr < 0)
		return 1e10f;

	float discrSqrt = sqrtf(discr);
	//float q = ((b < 0) ? (-b - discrSqrt) / 2.0f : (-b + discrSqrt)) / 2.0f;
	float q = (b > 0) ? -0.5 * (b + discrSqrt) : -0.5 * (b - discrSqrt);
	
	float t0 = q / a;
	float t1 = c / q;

	float distance = (t0 < t1) ? t0 : t1;
	return distance;
	
	/*float check = pow(Vec3Df::dotProduct(dir, (orig - center)), 2) - (orig - center).getSquaredLength() + (r*r);
	if (check < 0)
		return 1e10f;
	float a = sqrt(check);
	float b = -(Vec3Df::dotProduct(dir, (orig - center)));
	float d1 = b + a;
	float d2 = b - a;

	float distance = (d1 > d2) ? d2 : d1;
	return distance;*/
}

float Sphere::raytrace(const Vec3Df& orig, const Vec3Df& dir, Vec3Df* impact, Vec3Df* normal, Material** mat) {
	float dist = intersectSphere(orig, dir, center, radius);
	*impact = orig + dist * dir;
	*mat = &defaultMat;
	*normal = *impact - center;
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
