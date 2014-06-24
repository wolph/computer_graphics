/*
 * Material.cpp
 *
 *  Created on: 12 Jun 2014
 *      Author: rick
 */

#include "Material.hpp"

Material::Material(){
    cleanup();
}

void Material::cleanup() {
    name = "empty";
	Tr = 0;
	n = 0;
}

bool Material::is_valid(void) {
	return Kd.getLength() > 0 || Ka.getLength() > 0 || Ks.getLength() > 0 || Tr > 0;
}