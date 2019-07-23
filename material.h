#ifndef MATERIAL_H
#define MATERIAL_H

#include "vec3.h"
#include "color.h"

class Colorizer;
class PlainColorizer;

struct Material {
  Material() :
    color_(colors::BLACK), colorizer(0), ambient(0), diffuse(0), reflect(0), roughness(0) {}

  Material(const Colorizer* colorizer, float ambient=0.1, float diffuse=1.0, float reflect=0.5, float roughness=0)
  : color_(colors::PURPLE), colorizer(colorizer), ambient(ambient), diffuse(diffuse), reflect(reflect), roughness(roughness) {
  }

  Material(const Color& color, float ambient=0.1, float diffuse=1.0, float reflect=0.5, float roughness=0)
  : color_(color), colorizer(0), ambient(ambient), diffuse(diffuse), reflect(reflect), roughness(roughness) {
  }

  ~Material();

  Color color(const vec3& v) const;

  Color color_;
  const Colorizer* colorizer = 0;
  float ambient;
  float diffuse;
  float reflect;
  float roughness;
};

#endif
