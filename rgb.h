#ifndef RGB_H
#define RGB_H

#include <vector>
#include "color.h"

typedef unsigned char byte;

struct RGB {
  byte r = 0, g = 0, b = 0;

  RGB() {
    r = g = b = 0;
  }

  RGB(byte r, byte g, byte b) : r(r), g(g), b(b) {}

  RGB(const Color& color) : r(color.r * 255), g(color.g * 255), b(color.b * 255) {
    if (color.r < 0) r = 0;
    if (color.r > 1) r = 255;
    if (color.g < 0) g = 0;
    if (color.g > 1) g = 255;
    if (color.b < 0) b = 0;
    if (color.b > 1) b = 255;
  }

  operator Color() const {
    return Color(float(r) / 255, float(g) / 255, float(b) / 255);
  }
};

#endif
