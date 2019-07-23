#ifndef IMAGE_H
#define IMAGE_H

#include "matrix.h"
#include "color.h"
#include "math.h"

struct Filter {
  static const int radius_x = 16;
  static const int radius_y = 16;
  static const int width = radius_x * 2 + 1;
  static const int height = radius_y * 2 + 1;

  float data[height][width] = {0};

  static Filter id() {
    Filter res;
    res.data[radius_y][radius_x] = 1;
    return res;
  }

  void inormalize() {
    float sum = 0;
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        sum += data[y][x];
      }
    }
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        data[y][x] /= sum;
      }
    }
  }

  static Filter dist() {
    Filter res;
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        int y_dist_from_center = y - radius_y;
        int x_dist_from_center = x - radius_x;
        float dist_2 = y_dist_from_center * y_dist_from_center + x_dist_from_center * x_dist_from_center;
        float dist = sqrt(dist_2);
        res.data[y][x] = 1 / (1 + dist);
      }
    }
    return res;
  }

  static Filter dist2() {
    Filter res;
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        int y_dist_from_center = y - radius_y;
        int x_dist_from_center = x - radius_x;
        float dist_2 = y_dist_from_center * y_dist_from_center + x_dist_from_center * x_dist_from_center;
        res.data[y][x] = 1 / (1 + dist_2);
      }
    }
    return res;
  }

  static Filter exp_neg_dist_2_n(float n) {
    Filter res;
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        int y_dist_from_center = y - radius_y;
        int x_dist_from_center = x - radius_x;
        float dist_2 = y_dist_from_center * y_dist_from_center + x_dist_from_center * x_dist_from_center;
        dist_2 *= n;
        res.data[y][x] = exp(-dist_2);
      }
    }
    return res;
  }

  static Filter angled() {
    Filter res;
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        int dx = x - radius_x;
        int dy = y - radius_y;
        float angle = 0;
        if (dx > 0) {
          angle = atan(dy / dx);
        }
        angle = fmod(angle, M_PI / 32);
        if (angle < 0) angle += M_PI / 32;
        float dist2 = dx * dx + dy * dy;
        res.data[y][x] = angle / (1 + dist2);
      }
    }
    return res;
  }

  // Alpha relative to the center of the filter.
  float alpha(int dx, int dy) const {
    if (dx < -radius_x || dx > radius_x || dy < -radius_y || dy > radius_y) {
      std::cerr << "Illegal offset to filter" << std::endl;
    }
    return data[dy + radius_y][dx + radius_x];
  }
};

class Image : public Matrix<Color> {
public:
  Image(int width, int height) :
  Matrix<Color>(width, height) {}

  void convolve(const Filter& filter, Image* res) {
    if (!is_compatible(*res)) {
      std::cerr << "ERROR: Trying to convolve into incompatible Image: ("
                << width_ << ", " << height_ << ") (" << res->width_ << ", "
                << res->height_ << ")" << std::endl;
      return;
    }
    for (int y = 0; y < height_; ++y) {
      for (int x = 0; x < width_; ++x) {
          applyFilter(x, y, filter, res);
      }
    }
  }

  void applyFilter(int x, int y, const Filter& filter, Image* res) {
    Color c;
    for (int dy = -filter.radius_y; dy <= filter.radius_y; ++dy) {
      for (int dx = -filter.radius_x; dx <= filter.radius_x; ++dx) {
        if (is_safe(x + dx, y + dy)) {
          c += (*this)(x + dx, y + dy) * filter.alpha(dx, dy);
        }
      }
    }
    (*res)(x, y) = c;
  }

  void bloom(Matrix<RGB>* img) {
    if (!is_compatible(*img)) {
      std::cerr << "ERROR: Trying to bloom into incompatible Image: ("
                << width_ << ", " << height_ << ") (" << img->width_ << ", "
                << img->height_ << ")" << std::endl;
      return;
    }
    Image bloomed(width_, height_);
    Filter filter = Filter::dist2();
    filter.inormalize();
    convolve(filter, &bloomed);
    for (int i = 0; i < size_; ++i) {
      Color color = bloomed(i);
      RGB rgb = color.toRGB();
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

  void drawVerticalLine(int x, const Color& color) {
    for (int y = 0; y < height_; ++y) {
      (*this)(x, y) = color;
    }
  }

  void drawHorizontalLine(int y, const Color& color) {
    for (int x = 0; x < width_; ++x) {
      (*this)(x, y) = color;
    }
  }

  void drawSquare(int x0, int y0, int x1, int y1, const Color& color) {
    for (int y = y0; y < y1; ++y) {
      for (int x = x0; x < x1; ++x) {
        (*this)(x, y) = color;
      }
    }
  }

  void drawSquareCrissCross(int x0, int y0, int x1, int y1, int square_size, const Color& color1, const Color& color2) {
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
