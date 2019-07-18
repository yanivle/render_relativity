#include "material.h"

#include "colorizer.h"

Material::~Material() {
}

FRGB Material::rgb(const vec3& v) const {
  if (colorizer == 0) return color;
  return colorizer->rgb(v);
}
