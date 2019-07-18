#ifndef IMAGE_H
#define IMAGE_H

#include "matrix.h"
#include "rgb.h"

class Image : public Matrix<FRGB> {
public:
  Image(int width, int height) :
  Matrix<FRGB>(width, height) {}

  void bloom(Matrix<RGB>* img) {
    if (!is_compatible(*img)) {
      std::cerr << "ERROR: Trying to bloom into incompatible Image: ("
                << width_ << ", " << height_ << ") (" << img->width_ << ", "
                << img->height_ << ")" << std::endl;
      return;
    }
    for (int i = 0; i < size_; ++i) {
      FRGB frgb = (*this)(i);
      RGB rgb = frgb.toRGB();
      (*img)(i) = rgb;
    }
  }

  void save(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    (void) fprintf(fp, "P6\n%d %d\n255\n", width_, height_);

    Matrix<RGB> bloomed(width_, height_);
    bloom(&bloomed);

    for (int i = 0; i < size_; ++i) {
      RGB rgb = bloomed(i);
      (void) fwrite(&rgb, 1, 3, fp);
    }
    (void) fclose(fp);
  }

  void drawVerticalLine(int x, const FRGB& color) {
    for (int y = 0; y < height_; ++y) {
      (*this)(x, y) = color;
    }
  }

  void drawHorizontalLine(int y, const FRGB& color) {
    for (int x = 0; x < width_; ++x) {
      (*this)(x, y) = color;
    }
  }

  void drawSquare(int x0, int y0, int x1, int y1, const FRGB& color) {
    for (int y = y0; y < y1; ++y) {
      for (int x = x0; x < x1; ++x) {
        (*this)(x, y) = color;
      }
    }
  }

  void drawSquareCrissCross(int x0, int y0, int x1, int y1, int square_size, const FRGB& color1, const FRGB& color2) {
    for (int y = y0; y < y1; ++y) {
      for (int x = x0; x < x1; ++x) {
        if ((x + y) / square_size % 2 == 0) {
          (*this)(x, y) = color1;
        } else {
          (*this)(x, y) = color2;
        }
      }
    }
  }
};

#endif
