#ifndef COLORIZER_H
#define COLORIZER_H

#include "range.h"
#include "perlin_noise.h"

class Colorizer {
public:
  virtual FRGB rgb(const vec3& v) const = 0;
  virtual ~Colorizer() {}
};

struct ParametrizableSurface {
  virtual void coordinates(const vec3& vec, float* u, float* v) const = 0;
  virtual ~ParametrizableSurface() {}
};

inline float posfmodf(const float& f, const float& m) {
  float r = fmodf(f, m);
  if (r < 0) r += m;
  return r;
}

inline bool checkerboard(float a, float b, float scale) {
  return ((posfmodf(a * scale, 1) > 0.5) ^ (posfmodf(b * scale, 1) > 0.5));
}

class CheckerboardColorizer : public Colorizer {
public:
  CheckerboardColorizer(const FRGB& color1, const FRGB& color2, float scale)
    : color1(color1), color2(color2), scale(scale), surface(0) {}

  void setSurface(const ParametrizableSurface* surface) {
    this->surface = surface;
  }

  FRGB rgb(const vec3& vec) const {
    float u, v;
    surface->coordinates(vec, &u, &v);
    if (checkerboard(u, v, scale)) {
      return color2;
    }
    return color1;
  }
private:
  FRGB color1, color2;
  float scale;
  const ParametrizableSurface* surface;
};

class PerlinNoiseColorizer : public Colorizer {
public:
  PerlinNoiseColorizer(const FRGB& color1, const FRGB& color2, float scale)
    : color1(color1), color2(color2), scale(scale), surface(0) {}

  void setSurface(const ParametrizableSurface* surface) {
    this->surface = surface;
  }

  FRGB rgb(const vec3& vec) const {
    float u, v;
    surface->coordinates(vec, &u, &v);
    float alpha = perlin.noise(u * scale, v * scale, 0);
    return interpolate_rgbs(alpha, color1, color2);
  }
private:
  FRGB color1, color2;
  float scale;
  const ParametrizableSurface* surface;
  PerlinNoise perlin;
};

#endif
