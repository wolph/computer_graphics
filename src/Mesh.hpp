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

/************************************************************
 * Class de maillage basique
 ************************************************************/
class Mesh{
public:
    //this is relevant for you:
    //Vertices are the vertex positions, textures, and normals of the mesh.
    vector<Vertex> vertices;
    //this is relevant for you:
    //texCoords are the texture coordinates, these are DIFFERENT indices in triangles.
    //in the current version, if you use textures, then you have to use texture coords everywhere...
    //I might send an update of the code to change this.
    //for convenience, Vec3Df is used, although only 2D tex coordinates are read corresponding to the x,y entry of Vec3Df.
    vector<Texture> texcoords;
    //Triangles are the indices of the vertices involved in a triangle.
    //a triplet corresponds to one triangle.
    //A Triangle contains the indeces of the three vertices that are neighboring
    vector<Triangle> triangles;
    //These are the material properties
    //each triangle (!) has a material. Use the triangle index to receive a material INDEX
    vector<unsigned int> triangleMaterials;
    //using the material index, you can then recover the material from this vector
    //the class material is defined just above
    vector<Material> materials;

    const string name;

    inline Mesh(const string name, const string filename): name(name){
        loadMesh(filename, true);
    }

	inline const void draw() {}
	inline const void drawSmooth() {}

private:
    bool loadMesh(string filename, bool randomizeTriangulation);
    bool loadMtl(const char * filename,
            map<string, unsigned int> & materialIndex);
};
