#ifndef IMAGE_H
#define IMAGE_H

#include "array2d.h"
#include "color.h"
#include "math.h"
#include "fft.h"
#include "logging.h"
#include "rgb.h"

class Image : public Array2D<Color> {
public:
  Image(int width, int height) :
    Array2D<Color>(width, height) {}

  enum Channel {
    RED, GREEN, BLUE
  };

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

  void save(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    CHECK(fp != 0) << "cannot open file '" << filename << "'.";
    (void) fprintf(fp, "P6\n%d %d\n255\n", (int)width_, (int)height_);

    for (int i = 0; i < size_; ++i) {
      RGB rgb((*this)(i));
      (void) fwrite(&rgb, 1, 3, fp);
    }
    (void) fclose(fp);
  }
};

#endif
