#ifndef FRGB_H
#define FRGB_H

#include <vector>

typedef unsigned char byte;

struct RGB {
  byte r, g, b;

  RGB() {
    r = g = b = 0;
  }

  RGB(byte r, byte g, byte b) : r(r), g(g), b(b) {}
};

struct FRGB {
  float r, g, b;

  FRGB() {
    r = g = b = 0;
  }

  RGB toRGB() const {
    FRGB clamped = clamp(r, g, b, 255);
    return RGB(clamped.r, clamped.g, clamped.b);
  }

  float luminance() const {
    return 0.2126 * r + 0.7152 * g + 0.0722 * b;
  }

  // RGB toRGBWithBloom(RGB* bloom, float *bloom_luminance) const {
  //   FRGB clamped = clamp(r, g, b, 255);
  //   FRGB leftover = clamp(r - clamped.r, g - clamped.g, b - clamped.b);
  //   *bloom = leftover.toRGB();
  //   *bloom_luminance = leftover.luminance();
  //   return RGB(clamped.r, clamped.g, clamped.b);
  // }

  FRGB(byte r, byte g, byte b) {
    this->r = r;
    this->g = g;
    this->b = b;
  }

  static
  FRGB clamp(float r, float g, float b, float max=255) {
    if (r < 0) r = 0;
    if (r > max) r = max;
    if (g < 0) g = 0;
    if (g > max) g = max;
    if (b < 0) b = 0;
    if (b > max) b = max;
    return FRGB(r, g, b);
  }

  FRGB operator* (float alpha) const {
    return clamp(r * alpha, g * alpha, b * alpha);
  }

  FRGB operator+ (const FRGB& other) const {
    return clamp(r + other.r, g + other.g, b + other.b);
  }

  static FRGB average(const std::vector<FRGB>& rgbs) {
    FRGB res;
    for (int i = 0; i < rgbs.size(); ++i) {
      res = res + rgbs[i];
    }
    return res * (1 / rgbs.size());
  }

private:
};

namespace colors {
  const FRGB BLACK(0, 0, 0);
  const FRGB WHITE(255, 255, 255);
  const FRGB RED(255, 0, 0);
  const FRGB GREEN(0, 255, 0);
  const FRGB BLUE(0, 0, 255);
  const FRGB YELLOW(255, 255, 0);
  const FRGB PURPLE(255, 0, 255);
  const FRGB CYAN(0, 255, 255);
  const FRGB GRAY(128, 128, 128);
};

#endif
