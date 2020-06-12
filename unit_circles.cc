#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <atomic>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "filters.h"
#include "image.h"
#include "palette.h"
#include "progress.h"
#include "rand_utils.h"
#include "range.h"
#include "vec3.h"

void drawUnitCircle(Image& image, const Palette& palette, float p,
                    float exp_falloff = -50) {
  float one_over_p = 1 / p;
  for (int yp = 0; yp < image.height(); ++yp) {
    for (int xp = 0; xp < image.width(); ++xp) {
      float x = interpolate(xp, Range(0, image.width()), Range(-1, 1));
      float y = interpolate(yp, Range(0, image.height()), Range(-1, 1));
      x *= 1.2;
      y *= 1.2;
      float norm = powf(powf(abs(x), p) + powf(abs(y), p), one_over_p);
      if (p >= 100) {
        norm = std::max(abs(x), abs(y));
      }
      float alpha = norm;
      if (exp_falloff != 0) {
        alpha = exp(exp_falloff * abs(1 - norm));
      }
      image(xp, yp) += palette.color(alpha);
    }
  }
}

void drawScale(const Palette& palette) {
  Image img(1000, 100);
  for (int y = 0; y < img.height(); ++y) {
    for (int x = 0; x < img.width(); ++x) {
      img(x, y) =
          palette.color(interpolate(x, Range(0, img.width()), Range(0, 1)));
    }
  }
  img.save("unit_circles_scale.ppm");
}

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);

  drawScale(Palette::Rainbow());

  Image img(1000, 1000);

  drawUnitCircle(img, Palette::Rainbow(), 1, 0);
  img.save("unit_circles_l1_norm.ppm");
  img.clear();
  drawUnitCircle(img, Palette::Rainbow(), 2, 0);
  img.save("unit_circles_l2_norm.ppm");
  img.clear();
  drawUnitCircle(img, Palette::Rainbow(), 3, 0);
  img.save("unit_circles_l3_norm.ppm");
  img.clear();
  drawUnitCircle(img, Palette::Rainbow(), 1000, 0);
  img.save("unit_circles_linf_norm.ppm");
  img.clear();

  // drawUnitCircle(img, Color(0, 1, 0), 2);
  // drawUnitCircle(img, Color(0, 0, 1), 1000);

  // img.save("unit_circles.ppm");

  // img.clear();
  // const int NUM_CIRCLES = 10;
  // for (int i = 1; i < NUM_CIRCLES; ++i) {
  //   float p = interpolate(i, Range(0, NUM_CIRCLES), Range(0, 1));
  //   float n = 1 + p * NUM_CIRCLES;
  //   float exp_cutoff = -200;
  //   drawUnitCircle(img, Color::HSV(p, 0.5, 1.0), n, exp_cutoff);
  // }
  // img.save("unit_circles2.ppm");

  // for (int i = 1; i < NUM_CIRCLES; ++i) {
  //   float p = interpolate(i, Range(0, NUM_CIRCLES), Range(0, 1));
  //   float n = p;
  //   float exp_cutoff = -20;
  //   drawUnitCircle(img, Color::HSV(p, 0.5, 1.0), n, exp_cutoff);
  // }
  // img.save("unit_circles3.ppm");

  return EXIT_SUCCESS;
}
