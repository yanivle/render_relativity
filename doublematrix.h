#ifndef DOUBLEMATRIX_H
#define DOUBLEMATRIX_H

#include "matrix.h"

class DoubleMatrix : public Matrix<double> {
public:
  DoubleMatrix(int width, int height) :
  Matrix<double>(width, height) {}

  double max() {
    double mx = -100000000;
    for (int i = 0; i < size_; ++i) {
      if ((*this)(i) > mx) mx = (*this)(i);
    }
    return mx;
  }

  double min() {
    double mn = 100000000;
    for (int i = 0; i < size_; ++i) {
      if ((*this)(i) < mn) mn = (*this)(i);
    }
    return mn;
  }

  void safe_inc(int x, int y) {
    if (is_safe(x, y)) (*this)(x, y)++;
  }

  void toImage(Image* img, const Palette& palette) {
    if (!is_compatible(*img)) {
      std::cerr << "ERROR: Trying to convert incompatible DoubleMatrix to Image: ("
                << width_ << ", " << height_ << ") (" << img->width_ << ", "
                << img->height_ << std::endl;
      return;
    }
    double mx = max();
    double mn = min();
    std::cout << "Max: " << mx << ", min: " << mn << std::endl;
    for (int i = 0; i < size_; ++i) {
      double v = (*this)(i);
      double alpha = get_relative(mn, mx, v);
      (*img)(i) = palette.color(alpha);
    }
  }

private:
  double get_relative(double mn, double mx, double val) {
    return (val - mn) / (mx - mn);
  }

  byte get_relative_byte(double mn, double mx, double val) {
    return get_relative(mn, mx, val) * 255;
  }
};

#endif
