#ifndef RGB_H
#define RGB_H

#include <vector>

typedef unsigned char byte;

struct RGB {
  byte r = 0, g = 0, b = 0;

  RGB() {
    r = g = b = 0;
  }

  RGB(byte r, byte g, byte b) : r(r), g(g), b(b) {}
};

#endif
