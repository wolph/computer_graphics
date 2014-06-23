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

Material::Material(const Material & m){
	*this = m;
}

Material& Material::operator=(const Material& m){
	Kd_ = m.Kd_;
	Kd_is_set_ = m.Kd_is_set_; // diffuse
	Ka_ = m.Ka_;
	Ka_is_set_ = m.Ka_is_set_; // ambient
	Ks_ = m.Ks_;
	Ks_is_set_ = m.Ks_is_set_; // specular

	Ns_ = m.Ns_;
	Ns_is_set_ = m.Ns_is_set_; // specular
	Ni_ = m.Ni_;
	Ni_is_set_ = m.Ni_is_set_; // specular

	Tr_ = m.Tr_;
	Tr_is_set_ = m.Tr_is_set_; // transperency
	illum_ = m.illum_;
	name_ = m.name_;
	return (*this);
}

void Material::cleanup(){
	Kd_is_set_ = false;
	Ka_is_set_ = false;
	Ks_is_set_ = false;
	Ns_is_set_ = false;
	Ni_is_set_ = false;
	Tr_is_set_ = false;
	illum_is_set_ = false;
	name_ = "empty";
}

bool Material::is_valid(void) const{
	return Kd_is_set_ || Ka_is_set_ || Ks_is_set_ || Tr_is_set_;
}

bool Material::has_Kd(void){
	return Kd_is_set_;
}
bool Material::has_Ka(void){
	return Ka_is_set_;
}
bool Material::has_Ks(void){
	return Ks_is_set_;
}
bool Material::has_Ns(void){
	return Ns_is_set_;
}
bool Material::has_Ni(void){
	return Ni_is_set_;
}
bool Material::has_illum(void){
	return illum_is_set_;
}
bool Material::has_Tr(void){
	return Tr_is_set_;
}

void Material::set_Kd(float r, float g, float b){
	Kd_ = Vec3Df(r, g, b);
	Kd_is_set_ = true;
}

void Material::set_Ka(float r, float g, float b){
	Ka_ = Vec3Df(r, g, b);
	Ka_is_set_ = true;
}

void Material::set_Ks(float r, float g, float b){
	Ks_ = Vec3Df(r, g, b);
	Ks_is_set_ = true;
}

void Material::set_Ns(float r){
	Ns_ = r;
	Ns_is_set_ = true;
}

void Material::set_Ni(float r){
	Ni_ = r;
	Ni_is_set_ = true;
}

void Material::set_illum(int r){
	illum_ = r;
	illum_is_set_ = true;
}

void Material::set_Tr(float t){
	Tr_ = t;
	Tr_is_set_ = true;
}

void Material::set_textureName(const std::string & s) //name of the texture image file
		{
	textureName_ = s;
}

void Material::set_name(const std::string & s){
	name_ = s;
}

const Vec3Df & Material::Kd(void) const{
	//diffuse
	return Kd_;
}
const Vec3Df& Material::Ka(void) const{
	//ambiant
	return Ka_;
}
const Vec3Df& Material::Ks(void) const{
	//specular
	return Ks_;
}

const float Material::Ni(void) const{
	return Ni_;
}

const float Material::Ns(void) const{
	//shininess
	return Ns_;
}

const int Material::illum(void) const{
	//illumination
	return illum_;
}
const float Material::Tr(void) const{
	//transparency
	return Tr_;
}
const std::string & Material::textureName() const{
	//name of the texture image file
	return textureName_;
}

const std::string & Material::name() const{
	//name of the material
	return name_;
}
