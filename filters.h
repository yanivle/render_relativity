#ifndef FILTER_H
#define FILTER_H

#include "array2d.h"
#include <iostream>
#include "math.h"
#include "image.h"
#include "fft.h"

typedef Array2D<fft::Complex> ComplexArray2D;

namespace filters {
  ComplexArray2D Id(int width, int height) {
    ComplexArray2D res(width, height);
    res(0, 0) = 1;
    fft::fft2d(res);
    return res;
  }

  ComplexArray2D Bloom(int width, int height) {
    ComplexArray2D res(width, height);
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        int dx = std::abs(x - width / 2);
        int dy = std::abs(y - height / 2);
        res(x, y) = 1 / (1. + dx * dx + dy * dy);
        if (dx > 16 || dy > 16) {
          res(x, y) = 0;
        }
      }
    }
    res = res.rotate(width / 2, height / 2);
    res.inormalize();
    res(0, 0) += 1;
    res /= 2;

    fft::fft2d(res);
    return res;
  }

  void Convolve(Image& image, const ComplexArray2D& filter) {
    for (int channel_id = Image::RED; channel_id <= Image::BLUE; ++channel_id) {
      Image::Channel channel = Image::Channel(channel_id);
      ComplexArray2D c = image.getChannel(channel);
      fft::fft2d(c);
      c *= filter;
      fft::ifft2d(c);
      Array2D<float> arr = c.transform<float>([](fft::Complex c) {return c.real();});
      image.setChannel(channel, arr);
    }
  }
};

#endif
