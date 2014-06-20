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

using namespace std;

class Material{

public:

    Material();
    Material(const Material & m);
    Material& operator=(const Material& m);
    void cleanup();
    bool is_valid(void) const;
    bool has_Kd(void);
    bool has_Ka(void);
    bool has_Ks(void);
    bool has_Ns(void);
    bool has_Ni(void);
    bool has_illum(void);
    bool has_Tr(void);
    void set_Kd(float r, float g, float b);
    void set_Ka(float r, float g, float b);
    void set_Ks(float r, float g, float b);
    void set_Ns(float r);
    void set_Ni(float r);
    void set_illum(int r);
    void set_Tr(float t);
    void set_textureName(const std::string & s); //name of the texture image file
    void set_name(const std::string & s);
    const Vec3Df& Kd(void) const;
    const Vec3Df& Ka(void) const;
    const Vec3Df& Ks(void) const;
    const float Ni(void) const;
    const float Ns(void) const;
    const int illum(void) const;
    const float Tr(void) const;
    const std::string & textureName() const; //name of the texture image file
    const std::string & name() const; //name of the material

private:

    Vec3Df Kd_;
    bool Kd_is_set_; // diffuse
    Vec3Df Ka_;
    bool Ka_is_set_; // ambient
    Vec3Df Ks_;
    bool Ks_is_set_; // specular
    float Ns_;
    bool Ns_is_set_; // specular
    float Ni_;
    bool Ni_is_set_; // specular
    int illum_;
    bool illum_is_set_; //illumination model
    float Tr_;
    bool Tr_is_set_; // transperency
    std::string name_;
    std::string textureName_;
};

#endif /* MATERIAL_H_ */
