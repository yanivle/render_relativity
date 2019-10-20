#ifndef MATERIAL_H
#define MATERIAL_H

#include "vec3.h"
#include "color.h"

class Colorizer;
class PlainColorizer;

struct Material {
  Material() {}

  Material(const Colorizer* colorizer, float ambient=0.1, float diffuse=1.0, float reflect=0.5, float specular=0, float roughness=0)
  : color_(colors::PURPLE), colorizer(colorizer), ambient(ambient), diffuse(diffuse), reflect(reflect), specular(specular), roughness(roughness) {
  }

  Material(const Color& color, float ambient=0.1, float diffuse=1.0, float reflect=0.5, float specular=0, float roughness=0)
  : color_(color), colorizer(0), ambient(ambient), diffuse(diffuse), reflect(reflect), specular(specular), roughness(roughness) {
  }

  ~Material();

  Color color(const vec3& v) const;

  Color color_{};
  const Colorizer* colorizer = 0;
  float ambient = 0;
  float diffuse = 0;
  float reflect = 0;
  float specular = 0;
  float roughness = 0;
};

#endif
