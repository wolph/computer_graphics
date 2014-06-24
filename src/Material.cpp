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