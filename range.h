#ifndef RANGE_H
#define RANGE_H

struct Range {
  Range() {
    min = 0.0;
    max = 1.0;
    len = 1.0;
  }

  Range(double min, double max) :
  min(min), max(max) {
    len = max - min;
  }

  double min;
  double max;
  double len;
};

inline double interpolate(double val, const Range& src, const Range& dst) {
  return (((val - src.min) / src.len) * dst.len) + dst.min;
}

inline float interpolate_floats(float alpha, float f1, float f2) {
  return f2 * (1 - alpha) + f1 * alpha;
}

inline FRGB interpolate_rgbs(double alpha, const FRGB& a, const FRGB& b) {
  FRGB res;
  res.r = a.r * (1 - alpha) + b.r * alpha;
  res.g = a.g * (1 - alpha) + b.g * alpha;
  res.b = a.b * (1 - alpha) + b.b * alpha;
  return res;
}

#endif
