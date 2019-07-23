#include "material.h"

#include "colorizer.h"
#include "color.h"

Material::~Material() {
}

Color Material::color(const vec3& v) const {
  if (colorizer == 0) return color_;
  return colorizer->color(v);
}
