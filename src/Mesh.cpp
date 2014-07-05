#include "Mesh.hpp"
#include <ctime>
#include <fstream> /* load mesh */

const unsigned int LINE_LEN = 256;

bool Mesh::loadMesh(string filename, bool randomizeTriangulation){
	Timer timer;
	vertices.clear();
	triangles.clear();
	texcoords.clear();

	vector<unsigned int> vhandles;
	vector<unsigned int> texhandles;
	vector<vector<unsigned int>> tempTriangles;

	if (randomizeTriangulation)
		/* Randomization should use an actual _RANDOM_ thing */
		srand((unsigned int)time(NULL));

	materials.clear();
	Material defaultMat;
	defaultMat.Kd = Vec3Df(0.5f, 0.5f, 0.5f);
	defaultMat.Ka = Vec3Df(0.f, 0.f, 0.f);
	defaultMat.Ks = Vec3Df(0.5f, 0.5f, 0.5f);
	defaultMat.Ns = 96.7f;

	defaultMat.illum = 2;
	defaultMat.name = "StandardMaterialInitFromTriMesh";
	materials.push_back(defaultMat);

	map<string, unsigned int> materialIndex;
	char s[LINE_LEN];
	float x, y, z;

	//we replace the \ by /
	string realFilename(filename);
	for (unsigned int i = 0; i < realFilename.length(); ++i){
		if (realFilename[i] == '\\')
			realFilename[i] = '/';
	}

	vector<Vec3Df> normals;
	string matname;
	string path_;
	string temp(realFilename);
	int pos = (int)temp.rfind("/");

	if (pos < 0){
		path_ = "";
	}
	else {
		path_ = temp.substr(0, pos + 1);
	}
	memset(&s, 0, LINE_LEN);

	ifstream in(filename);

	string token;
	in >> token;

	while (in) {
		// comment
		if (token[0] == '#') {
			getline(in, token);
		}

		// material file
		else if (token == "mtllib") {
			string path;
			char dummy;
			in.read(&dummy, 1);
			getline(in, path);
			loadMtl((path_ + path).c_str(), materialIndex);
		}

		// use material
		else if (token == "usemtl") {
			in >> matname;
			if (materialIndex.find(matname) == materialIndex.end()) {
				printf("unknown material");
				matname = "";
			}
		}

		// vertex
		else if (token == "v") {
			in >> x >> y >> z;
			vertices.push_back(Vec3Df(x, y, z));
		}

		// texture coord
		else if (token == "vt") {
			float u, v;
			in >> u >> v;
			texcoords.push_back(Vec3Df(u, v, 0));
		}

		// normal
		else if (token == "vn") {
			in >> x >> y >> z;
			/* nothing */
		}

		// face
		else if (token == "f") {
			// simulate old because of legacy code...
			string ss;
			getline(in, ss);
			ss = "f " + ss;
			strcpy(s, ss.c_str());

			// <legacy code>
			int component(0), nV(0);
			bool endOfVertex(false);
			char *p0, *p1(s + 2); //place behind the "f "

			vhandles.clear();
			texhandles.clear();

			while (*p1 == ' ')
				++p1; // skip white-spaces

			while (p1){
				p0 = p1;

				// overwrite next separator

				// skip '/', '\n', ' ', '\0', '\r' <-- don't forget Windows
				while (*p1 != '/' && *p1 != '\r' && *p1 != '\n' && *p1 != ' '
					&& *p1 != '\0')
					++p1;

				// detect end of vertex
				if (*p1 != '/')
					endOfVertex = true;

				// replace separator by '\0'
				if (*p1 != '\0'){
					*p1 = '\0';
					p1++; // point to next token
				}

				// detect end of line and break
				if (*p1 == '\0' || *p1 == '\n')
					p1 = 0;

				// read next vertex component
				if (*p0 != '\0'){
					switch (component){
					case 0: // vertex
					{
								int tmp = atoi(p0) - 1;
								vhandles.push_back(tmp);
					}
						break;

					case 1: // texture coord
					{
								int tmp = atoi(p0) - 1;
								texhandles.push_back(tmp);
					}
						break;

					case 2: // normal
						//assert(!vhandles.empty());
						//assert((unsigned int)(atoi(p0)-1) < normals.size());
						//_bi.set_normal(vhandles.back(), normals[atoi(p0)-1]);
						break;
					}
				}

				++component;

				if (endOfVertex){
					component = 0;
					nV++;
					endOfVertex = false;
				}
			}

			if (vhandles.size() != texhandles.size())
				texhandles.resize(vhandles.size(), 0);

			unsigned int mat = 0;

			if (matname[0] == '\0')
				/* default material */
				mat = 0;
			else if (!materialIndex.count(matname))
				printf("Material %s not found!\n", matname.c_str());
			else
				mat = materialIndex.find(matname)->second;

			if (vhandles.size() > 3){
				//model is not triangulated, so let us do this on the fly...
				//to have a more uniform mesh, we add randomization
				unsigned int k = (false) ? (rand() % vhandles.size()) : 0;
				for (unsigned int i = 0; i < vhandles.size() - 2; ++i){
					const int v0 = (k + 0) % vhandles.size();
					const int v1 = (k + i + 1) % vhandles.size();
					const int v2 = (k + i + 2) % vhandles.size();

					const int t0 = (k + 0) % vhandles.size();
					const int t1 = (k + i + 1) % vhandles.size();
					const int t2 = (k + i + 2) % vhandles.size();

					tempTriangles.push_back(vector<unsigned int> {vhandles[v0],
						vhandles[v1], vhandles[v2], texhandles[t0],
						texhandles[t1], texhandles[t2],
						mat});
					triangleMaterials.push_back(mat);
				}
			}
			else if (vhandles.size() == 3){
				triangleMaterials.push_back(mat);
				tempTriangles.push_back(vector<unsigned int> {vhandles[0],
					vhandles[1], vhandles[2], texhandles[0], texhandles[1],
					texhandles[2], mat});

			}
			else{
				printf(
					"TriMesh::LOAD: Unexpected number of face vertices (<3). Ignoring face\n");
			}
			// </legacy code>
		}
		in >> token;
	}
	printf("Done reading file in %.3f seconds\n", timer.next().count());

	unsigned int vsize = (int)vertices.size();

	/* WARNING!
	The double loop looks useless but it's needed for normal calculation.

	1. run through the triangles and add the triangle normals to the vertices
	2. run through the vertices to normalize
	3. run through the triangles to create the triangles with the correct
	vertex normals
	*/

	// make triangles
	printf("Making triangles...\n");
	for (auto& triangle : tempTriangles){
		bool error = false;
		for (int i = 0; i<3; i++){
			if (triangle[i] >= vsize){
				printf("This model is broken, it's referring to non-existing vertice with index %d\n", triangle[i]);
				error = true;
				break;
			}
		}
		if (error)continue;

		Triangle t(vertices[triangle[0]], vertices[triangle[1]],
			vertices[triangle[2]], materials[triangle[3]]);

		for (int i = 0; i<3; i++)
			vertices[triangle[i]].normal += t.normal;
	}

	for (auto& vertex : vertices)
		vertex.normal.normalize();

	printf("Calculated vertex normals in %.3f seconds\n", timer.next().count());

	for (auto& triangle : tempTriangles){
		bool error = false;
		for (int i = 0; i<3; i++){
			if (triangle[i] >= vsize){
				printf("This model is broken, it's referring to non-existing vertice with index %d\n", triangle[i]);
				error = true;
				break;
			}
		}
		if (!error){
			if (!texcoords.empty()){
				triangles.push_back(Triangle(vertices[triangle[0]], vertices[triangle[1]],
					vertices[triangle[2]], texcoords[triangle[3]],
					texcoords[triangle[4]], texcoords[triangle[5]], materials[triangle[6]]));

			}
			else{
				triangles.push_back(Triangle(vertices[triangle[0]], vertices[triangle[1]],
					vertices[triangle[2]], materials[triangle[6]]));
			}

		}
	}
	printf("Done making triangles in %.3f seconds\n", timer.next().count());

	return true;
}

bool Mesh::loadMtl(const char * filename, map<string, unsigned int> & materialIndex) {
	ifstream ifStream(filename, ifstream::in);
	float f1, f2, f3;
	Material mat;
	do {
		std::string word;
		ifStream >> word;
		if (word == "#") { //skip comment
			ifStream.ignore(99999999/*LINE_LEN*/, '\n');
		}
		else if (word == "newmtl"){

			/*if name is empty we haven't (correctly) loaded a material yet,
			* so no need to save. This is probably the first occurrence of newmtl*/
			if (mat.name != "empty") {
				materials.push_back(mat);
				materialIndex[mat.name] = (unsigned int)materials.size() - 1;
			}
			mat.cleanup();
			ifStream >> mat.name;
			//printf("new material found: %s", mat.name);
		}
		else if (word == "Kd") {
			ifStream >> f1 >> f2 >> f3;
			mat.Kd = Vec3Df(f1, f2, f3);
		}
		else if (word == "Ks") {
			ifStream >> f1 >> f2 >> f3;
			mat.Ks = Vec3Df(f1, f2, f3);
		}
		else if (word == "Ka") {
			ifStream >> f1 >> f2 >> f3;
			mat.Kd = Vec3Df(f1, f2, f3);
		}
		else if (word == "Ns") {
			ifStream >> mat.Ns;
		}
		else if (word == "Ni") {
			ifStream >> mat.Ni;
		}
		else if (word == "d" || word == "Tr") {
			ifStream >> mat.Tr;
		}
		else if (word == "illum") {
			ifStream >> mat.illum;
			if (mat.illum >= 0) { mat.color = true; mat.ambient = false; }
			if (mat.illum >= 1) { mat.color = true; mat.ambient = true; }
			if (mat.illum >= 2) { mat.highlight = true; }
			if (mat.illum >= 3) { mat.reflection = true; }
			if (mat.illum >= 6) { mat.refraction = true; }
			if (mat.illum >= 9) { mat.refraction = true; mat.reflection = false; }
			if (mat.illum >= 10) { /* casts shadow on invisible surfaces*/ }
		}
		else if (word == "map_Ks") {
		}
		else if (word == "n") {
			ifStream >> mat.n;
		}
		else if (word == "map_Kd") {
			std::string t;
			ifStream >> t;
			try {
				mat.loadTexture(t);
			}
			catch (string s) {
				printf("%s\n", s.c_str());
				exit(-1);
			}
		}
		else if (word == "map_Ka") {
		}
		else if (word == "map_Ns") {
		}
		else if (word == "map_d") {
		}
		else if (word == "map_bump") {
		}
	} while (ifStream);
	ifStream.close();
	if (mat.name != "empty") {
		materials.push_back(mat);
		materialIndex[mat.name] = (unsigned int)materials.size() - 1;
	}
	printf("%d  materials loaded.\n", (int)materials.size());
	return true;
}
