/*
 * Material.cpp
 *
 *  Created on: 12 Jun 2014
 *      Author: rick
 */

#include "Material.hpp"

void Material::cleanup() {
	illum = 2;
	Ns = 1.0f;
	Ni = 0.3f;
    name = "empty";
	Tr = 0;
	n = 1;

	color = false;
	ambient = false;
	highlight = false;
	reflection = false;
	refraction = false;

}

bool Material::is_valid(void) {
	return Kd.getLength() > 0 || Ka.getLength() > 0 || Ks.getLength() > 0 || Tr > 0;
}

void Material::loadTexture(std::string textureName){
    // load texture
    FILE* fp;
    fp = fopen(("mesh/" + textureName).c_str(), "rb");
    if (fp == NULL){
        fp = fopen((textureName).c_str(), "rb");
        if (fp == NULL){
            cout << "Unable to open " << textureName << endl;
            return;
        }
    }
    cout << "Loading " << textureName << endl;

    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, fp);

    unsigned int width = *(int*)&info[18];
    unsigned int height = *(int*)&info[22];

    map_Kd.resize(width * height);

    unsigned int size = 3 * width * height;
    unsigned char* data = new unsigned char[size];
    fread(data, sizeof(unsigned char), size, fp);
    fclose(fp);

    for(unsigned int i = 0; i < width * height; i++){
        int j = i * 3;
        map_Kd[i][0] = data[j] / 255.f;
        map_Kd[i][1] = data[j+1] / 255.f;
        map_Kd[i][2] = data[j+2] / 255.f;
    }
}