#ifndef IMAGE_H
#define IMAGE_H

#include "array2d.h"
#include "color.h"
#include "fft.h"
#include "logging.h"
#include "math.h"
#include "rgb.h"

class Image : public Array2D<Color> {
 public:
  Image(int width, int height) : Array2D<Color>(width, height) {}

  Image(const Array2D<Color>& arr) : Array2D<Color>(arr) {}

  Image(const Array2D<Color>&& arr) : Array2D<Color>(std::move(arr)) {}

  enum Channel { RED, GREEN, BLUE };

  Array2D<float> getChannel(Channel channel) {
    Array2D<float> res(width(), height());
    for (int i = 0; i < size(); ++i) {
      switch (channel) {
        case Image::RED:
          res(i) = (*this)(i).r;
          break;
        case Image::GREEN:
          res(i) = (*this)(i).g;
          break;
        case Image::BLUE:
          res(i) = (*this)(i).b;
          break;
        default:
          std::cerr << "Illegal channel " << channel << std::endl;
          return res;
      }
    }
    return res;
  }

  void setChannel(Channel channel, const Array2D<float>& data) {
    for (int i = 0; i < size(); ++i) {
      switch (channel) {
        case RED:
          (*this)(i).r = data(i);
          break;
        case GREEN:
          (*this)(i).g = data(i);
          break;
        case BLUE:
          (*this)(i).b = data(i);
          break;
        default:
          std::cerr << "Illegal channel " << channel << std::endl;
          return;
      }
    }
  }

  void clearChannel(Channel channel) {
    for (int i = 0; i < size(); ++i) {
      switch (channel) {
        case RED:
          (*this)(i).r = 0;
          break;
        case GREEN:
          (*this)(i).g = 0;
          break;
        case BLUE:
          (*this)(i).b = 0;
          break;
        default:
          std::cerr << "Illegal channel " << channel << std::endl;
          return;
      }
    }
  }

  void save(const std::string& s) { save(s.c_str()); }
  void save(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    CHECK(fp != 0) << "cannot open file '" << filename << "'.";
    (void)fprintf(fp, "P6\n%d %d\n255\n", (int)width_, (int)height_);

    for (int i = 0; i < size_; ++i) {
      RGB rgb((*this)(i));
      (void)fwrite(&rgb, 1, 3, fp);
    }
    (void)fclose(fp);
  }

  static Image load(const std::string& s) { return load(s.c_str()); }

  static Image load(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    CHECK(fp != 0) << "cannot open file '" << filename << "'.";

    char p6[10];
    fscanf(fp, "%9s", p6);
    CHECK(std::string(p6) == "P6") << "Invalid header '" << p6 << "'";

    int width, height;
    fscanf(fp, "%d\n %d\n", &width, &height);
    CHECK(width > 0 && width < 10000) << "Invalid width" << width;
    CHECK(height > 0 && height < 10000) << "Invalid height" << height;

    int maximum;
    fscanf(fp, "%d\n", &maximum);

    int size = width * height;

    Image res(width, height);
    for (int i = 0; i < size; ++i) {
      RGB rgb;
      CHECK(fread(&rgb, 1, 3, fp) == 3) << "Can't read file";
      res(i) = rgb;
    }
    fclose(fp);

    return res;
  }
};

#endif
