#ifndef MATERIAL_H
#define MATERIAL_H

#include "rgb.h"
#include "vec3.h"

class Colorizer;
class PlainColorizer;

struct Material {
  Material() :
    color(colors::BLACK), colorizer(0), ambient(0), diffuse(0), reflect(0), roughness(0) {}

  Material(const Colorizer* colorizer, float ambient=0.1, float diffuse=1.0, float reflect=0.5, float roughness=0)
  : color(colors::PURPLE), colorizer(colorizer), ambient(ambient), diffuse(diffuse), reflect(reflect), roughness(roughness) {
  }

  Material(const FRGB& color, float ambient=0.1, float diffuse=1.0, float reflect=0.5, float roughness=0)
  : color(color), colorizer(0), ambient(ambient), diffuse(diffuse), reflect(reflect), roughness(roughness) {
  }

  ~Material();

  FRGB rgb(const vec3& v) const;

  FRGB color;
  const Colorizer* colorizer;
  float ambient;
  float diffuse;
  float reflect;
  float roughness;
};

#endif
