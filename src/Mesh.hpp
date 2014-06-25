#ifndef MESH_H
#define MESH_H

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

/************************************************************
 * Class de maillage basique
 ************************************************************/
class Mesh{
public:
    //this is relevant for you:
    //Vertices are the vertex positions, textures, and normals of the mesh.
    std::vector<Vertex> vertices;
    //this is relevant for you:
    //texCoords are the texture coordinates, these are DIFFERENT indices in triangles.
    //in the current version, if you use textures, then you have to use texture coords everywhere...
    //I might send an update of the code to change this.
    //for convenience, Vec3Df is used, although only 2D tex coordinates are read corresponding to the x,y entry of Vec3Df.
    std::vector<Texture> texcoords;
    //Triangles are the indices of the vertices involved in a triangle.
    //a triplet corresponds to one triangle.
    //A Triangle contains the indeces of the three vertices that are neighboring
    std::vector<Triangle> triangles;
    //These are the material properties
    //each triangle (!) has a material. Use the triangle index to receive a material INDEX
    std::vector<unsigned int> triangleMaterials;
    //using the material index, you can then recover the material from this vector
    //the class material is defined just above
    std::vector<Material> materials;

    const std::string name;

    inline Mesh(const std::string name, const std::string filename): name(name){
        loadMesh(filename, true);
    }

    const void draw();
    const void drawSmooth();

private:
    bool loadMesh(std::string filename, bool randomizeTriangulation);
    bool loadMtl(const char * filename,
            std::map<std::string, unsigned int> & materialIndex);
};

#endif // MESH_H
