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
    return res;
  }

  ComplexArray2D Blur(int width, int height) {
    ComplexArray2D res(width, height);
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        int dx = std::abs(x - width / 2);
        int dy = std::abs(y - height / 2);
        res(x, y) = 1 / (1. + dx * dx + dy * dy);
      }
    }
    res = res.rotate(width / 2, height / 2);
    res.inormalize();
    return res;
  }

  // TODO: move this to some general utils place (and I probably need this elsewhere).
  inline float pos_fmod(float x, float m) {
    float res = fmod(x, m);
    if (res < 0) {
      res += m;
    }
    return res;
  }

  ComplexArray2D Ray(int width, int height, int spokes=8, float rotation=(2 * M_PI) / 100, float ray_width=M_PI / 64) {
    ComplexArray2D res(width, height);
    float arc = (2 * M_PI) / spokes;
    float mid_arc = arc / 2;
    float ray_width_2 = ray_width * ray_width;

    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        float dx = x - width / 2;
        float dy = y - height / 2;
        float theta = atan(dy / (dx + 0.000000001));
        float dtheta = pos_fmod(theta + rotation, arc);
        float r2 = dx * dx + dy * dy;
        float angular_distance_from_mid_arc = (dtheta - mid_arc);
        float angular_distance_from_mid_arc_2 = angular_distance_from_mid_arc * angular_distance_from_mid_arc;

        if (angular_distance_from_mid_arc_2 < ray_width_2) {
          res(x, y) = 1 / (0.001 + r2);
        }
      }
    }
    res = res.rotate(width / 2, height / 2);
    res.inormalize();
    return res;
  }

  ComplexArray2D Bloom(int width, int height) {
    ComplexArray2D res(width, height);
    res = Id(width, height);
    res.iaverage(Ray(width, height));
    res.iaverage(Blur(width, height));
    return res;
  }

  void Convolve(Image& image, const ComplexArray2D& filter) {
    ComplexArray2D transformed_filter = filter;
    fft::fft2d_mt(transformed_filter);
    for (int channel_id = Image::RED; channel_id <= Image::BLUE; ++channel_id) {
      Image::Channel channel = Image::Channel(channel_id);
      ComplexArray2D c = image.getChannel(channel);
      fft::fft2d_mt(c);
      c *= transformed_filter;
      fft::ifft2d_mt(c);
      Array2D<float> arr = c.transform<float>([](fft::Complex c) {return c.real();});
      image.setChannel(channel, arr);
    }
  }
};

#endif
