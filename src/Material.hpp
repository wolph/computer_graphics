/*
 * Material.hpp
 *
 *  Created on: 12 Jun 2014
 *      Author: rick
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

/* fixed recursive includes */
class Material;

#include "Vec3D.hpp"
#include <string>
#include <vector>

using namespace std;

/**

Ka = Ambient Color
Kd = diffuse color
Ks = specular color
Ns = specular coefficient
Tr = transparency factor
n = density index
illum = options


0. Color on and Ambient off
1. Color on and Ambient on
2. Highlight on
3. Reflection on and Ray trace on
4. Transparency: Glass on, Reflection: Ray trace on
5. Reflection: Fresnel on and Ray trace on
6. Transparency: Refraction on, Reflection: Fresnel off and Ray trace on
7. Transparency: Refraction on, Reflection: Fresnel on and Ray trace on
8. Reflection on and Ray trace off
9. Transparency: Glass on, Reflection: Ray trace off
10. Casts shadows onto invisible surfaces
*/

class Material{

public:

    //Material();
    //Material(const Material & m);
    //Material& operator=(const Material& m);

    Vec3Df Kd; // diffuse color
	Vec3Df Ka; // ambient color
	Vec3Df Ks; // specular color
	float Ns; // specular exponent
	float Ni; // reflection index
	int illum; // illumination model
	float Tr; // transparency
	float n; // density index
    string name;
    string textureName;
    vector<Vec3Df> map_Kd;

	// illum
	bool color;
	bool ambient;
	bool highlight;
	bool reflection;
	bool refraction;

	bool is_valid();
	void cleanup();
    void loadTexture(string textureName);
};

#endif /* MATERIAL_H_ */

