#ifndef COLOR_H
#define COLOR_H

#include <sstream>
#include <vector>

struct Color {
  float r = 0, g = 0, b = 0;

  Color() {}

  Color(float r, float g, float b) : r(r), g(g), b(b) {}

  Color(int packed) {
    b = float(packed & 255) / 255;
    packed >>= 8;
    g = float(packed & 255) / 255;
    packed >>= 8;
    r = float(packed & 255) / 255;
  }

  float luminance() const {
    return 0.2126 * r + 0.7152 * g + 0.0722 * b;
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

  bool operator== (const Color& other) const {
    return r == other.r && g == other.g && b == other.b;
  }

  static Color average(const std::vector<Color>& colors) {
    Color res;
    for (int i = 0; i < colors.size(); ++i) {
      res += colors[i];
    }
    return res * (1 / colors.size());
  }

  std::string str() const {
    std::stringstream res;
    res << "Color(" << r << ',' << g << ',' << b << ')';
    return res.str();
  }

private:
};

inline std::ostream &operator<<(std::ostream &os, const Color &color) {
    return os << color.str();
}

namespace colors {
  const Color BLACK(0, 0, 0);
  const Color WHITE(1, 1, 1);
  const Color RED(1, 0, 0);
  const Color GREEN(0, 1, 0);
  const Color BLUE(0, 0, 1);
  const Color YELLOW(1, 1, 0);
  const Color PURPLE(1, 0, 1);
  const Color CYAN(0, 1, 1);
  const Color GRAY(0.5, 0.5, 0.5);
  const Color ORANGE(1, 0.647, 0);
  const Color CORAL(1, 0.498, 80);
};

#endif
