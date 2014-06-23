#include "Scene.hpp"
#include <cstdio>
#include <fstream>

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

float Scene::raytrace(const Vec3Df& orig, const Vec3Df& dir, Triangle** tr, Object** obj) {
	float close = 1e10f;
	*tr = 0;
	*obj = 0;
	for (unsigned int i = 0; i < objects.size(); i++) {
		Triangle* temp;
		float dist = objects[i]->raytrace(orig, dir, &temp);
		if (dist < close) {
			close = dist;
			*tr = temp;
			*obj = objects[i];
		}
	}
	return close;
}

void Scene::addLightPoint(Vec3Df& lightPos) {
	lights.push_back(lightPos);
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

float Object::raytrace(const Vec3Df& orig, const Vec3Df& dir, Triangle** tr) {
	return tree.collide(orig - pos, dir, tr);
}
