#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#include "Material.hpp"
#include "Triangle.hpp"
#include "Vertex.hpp"
#include "Texture.hpp"
#include <stdio.h>
#include <GL/glut.h>
#include <map>
#include <string.h>
#include <vector>
#include <string>
#include "Timer.hpp"
#include <fstream>

/**
 *	This is a mesh of polygons
 */
class Mesh {
public:
    //this is relevant for you:
    //Vertices are the vertex positions, textures, and normals of the mesh.
    vector<Vertex> vertices;
	
	// uv-coordinates
	vector<Vec3Df> texcoords;
	
	vector<Triangle> triangles;
    vector<int> triangleMaterials;

    vector<Material> materials;

    const string name;

    inline Mesh(const string name, const string filename): name(name){
        loadMesh(filename, true);
    }

private:
    bool loadMesh(string filename, bool randomizeTriangulation);
    bool loadMtl(const char * filename,
            map<string, unsigned int> & materialIndex);
};
