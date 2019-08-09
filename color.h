#ifndef COLOR_H
#define COLOR_H

#include "rgb.h"
#include <sstream>

struct Color {
  float r = 0, g = 0, b = 0;

  Color() {}

  RGB toRGB() const {
    Color clamped = clamp(r, g, b);
    return RGB(clamped.r, clamped.g, clamped.b);
  }

  float luminance() const {
    return 0.2126 * r + 0.7152 * g + 0.0722 * b;
  }

  Color(float r, float g, float b) {
    this->r = r;
    this->g = g;
    this->b = b;
  }

  static
  Color clamp(float r, float g, float b, float max=255) {
    if (r < 0) r = 0;
    if (r > max) r = max;
    if (g < 0) g = 0;
    if (g > max) g = max;
    if (b < 0) b = 0;
    if (b > max) b = max;
    return Color(r, g, b);
  }

  void operator*= (float alpha) {
    r *= alpha;
    g *= alpha;
    b *= alpha;
  }

  Color operator* (float alpha) const {
    return Color(r * alpha, g * alpha, b * alpha);
  }

  void operator/= (float alpha) {
    r /= alpha;
    g /= alpha;
    b /= alpha;
  }

  Color operator/ (float alpha) const {
    return Color(r / alpha, g / alpha, b / alpha);
  }

  void operator+= (const Color& other) {
    r += other.r;
    g += other.g;
    b += other.b;
  }

  Color operator+ (const Color& other) const {
    return Color(r + other.r, g + other.g, b + other.b);
  }

  static Color average(const std::vector<Color>& rgbs) {
    Color res;
    for (int i = 0; i < rgbs.size(); ++i) {
      res = res + rgbs[i];
    }
    return res * (1 / rgbs.size());
  }

  std::string str() const {
    std::stringstream res;
    res << "Color(" << r << ',' << g << ',' << b << ')';
    return res.str();
  }

private:
};

namespace colors {
  const Color BLACK(0, 0, 0);
  const Color WHITE(255, 255, 255);
  const Color RED(255, 0, 0);
  const Color GREEN(0, 255, 0);
  const Color BLUE(0, 0, 255);
  const Color YELLOW(255, 255, 0);
  const Color PURPLE(255, 0, 255);
  const Color CYAN(0, 255, 255);
  const Color GRAY(128, 128, 128);
  const Color ORANGE(255, 165, 0);
  const Color CORAL(255, 127, 80);
};

#endif
