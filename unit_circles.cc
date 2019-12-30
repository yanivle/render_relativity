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

void drawUnitCircle(Image& image, const Color& color, float p, float exp_falloff=-50) {
  float one_over_p = 1 / p;
  for (int yp = 0; yp < image.height(); ++yp) {
    for (int xp = 0; xp < image.width(); ++xp) {
      float x = interpolate(xp, Range(0, image.width()), Range(-1, 1));
      float y = interpolate(yp, Range(0, image.height()), Range(-1, 1));
      x *= 1.2;
      y *= 1.2;
      float norm = powf(powf(abs(x), p) + powf(abs(y), p), one_over_p);
      float alpha = exp(exp_falloff * abs(1 - norm));
      image(xp, yp) += color * alpha;
    }
  }
}

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);

  Image img(1000, 1000);

  drawUnitCircle(img, Color(1, 0, 0), 1);
  drawUnitCircle(img, Color(0, 1, 0), 2);
  drawUnitCircle(img, Color(0, 0, 1), 1000);

  img.save("unit_circles.ppm");

  img.clear();
  const int NUM_CIRCLES = 10;
  for (int i = 1; i < NUM_CIRCLES; ++i) {
    float p = interpolate(i, Range(0, NUM_CIRCLES), Range(0, 1));
    float n = 1 + p * NUM_CIRCLES;
    float exp_cutoff = -200;
    drawUnitCircle(img, Color::HSV(p, 0.5, 1.0), n, exp_cutoff);
  }
  img.save("unit_circles2.ppm");

  for (int i = 1; i < NUM_CIRCLES; ++i) {
    float p = interpolate(i, Range(0, NUM_CIRCLES), Range(0, 1));
    float n = p;
    float exp_cutoff = -20;
    drawUnitCircle(img, Color::HSV(p, 0.5, 1.0), n, exp_cutoff);
  }
  img.save("unit_circles3.ppm");

  return EXIT_SUCCESS;
}
