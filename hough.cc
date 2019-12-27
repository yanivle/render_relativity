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

ABSL_FLAG(std::string, image, "corner", "the name of the input image");
ABSL_FLAG(float, edges_threshold, 0.15, "the energy threshold for edges");
ABSL_FLAG(float, reverse_threshold, 0.55,
          "the energy threshold for the reverse map");
ABSL_FLAG(int, local_max_radius, 25,
          "the radius (square) to check for local maxima in");

float energy(Color c) { return c.r * c.r + c.g * c.g + c.b * c.b; }

typedef Array2D<float> farr2d;

const int skip = 1;
const int r_res = 400;
const int theta_res = 400;

float edginess(Image& img, int x, int y) {
  return abs(img(x, y).luminance() - img(x - 1, y).luminance()) +
         abs(img(x, y).luminance() - img(x, y - 1).luminance()) +
         abs(img(x, y).luminance() - img(x - 1, y - 1).luminance()) +
         abs(img(x, y).luminance() - img(x + 1, y - 1).luminance());
}

Array2D<float> detect_edges(Image& image) {
  Array2D<float> res(image.width(), image.height());
  for (int y = 1; y < image.height() - 1; ++y) {
    for (int x = 1; x < image.width() - 1; ++x) {
      res(x, y) = edginess(image, x, y);
      // Color diff1 = image(x, y) - image(x - 1, y);
      // Color diff2 = image(x, y) - image(x + 1, y);
      // Color diff3 = image(x, y) - image(x, y - 1);
      // Color diff4 = image(x, y) - image(x, y + 1);
      // res(x, y) = energy(diff1) + energy(diff2) + energy(diff3) +
      // energy(diff4);
    }
  }

  return res;
}

Array2D<float> hough_transform(Array2D<float>& edges) {
  float r_max = sqrt(2);

  Array2D<float> res(theta_res, r_res);

  Progress progress(edges.height());
  for (int yp = 0; yp < edges.height(); yp += skip) {
    progress.update(yp);
    float y = interpolate(yp, Range(0, edges.height()), Range(0, 1));
    for (int xp = 0; xp < edges.width(); xp += skip) {
      float val = edges(xp, yp);
      if (val == 0) continue;
      float x = interpolate(xp, Range(0, edges.width()), Range(0, 1));
      for (int thetap = 0; thetap < theta_res; ++thetap) {
        float theta = interpolate(thetap, Range(0, theta_res), Range(0, M_PI));
        float r = x * cos(theta) + y * sin(theta);
        int rp = interpolate(r, Range(0, r_max), Range(0, r_res));
        if (rp >= 0 && rp < r_res) {
          // TODO: hack! for some reason for r=0 this doesn't work...
          if (rp == 0) continue;
          res(thetap, rp) += val;
        }
      }
    }
  }
  progress.update(edges.height());
  progress.done();

  return res;
}

Array2D<float> reverse_hough_transform(Array2D<float>& hough, int width,
                                       int height) {
  Array2D<float> res(width, height);
  float r_max = sqrt(2);

  Progress progress(hough.width());
  for (int thetap = 0; thetap < hough.width(); thetap++) {
    progress.update(thetap);
    float theta = interpolate(thetap, Range(0, hough.width()), Range(0, M_PI));
    for (int rp = 0; rp < hough.height(); rp++) {
      float val = hough(thetap, rp);
      if (val == 0) continue;
      float r = interpolate(rp, Range(0, hough.height()), Range(0, r_max));
      if (theta > M_PI / 4 && theta < 3 * M_PI / 4) {
        for (int xp = 0; xp < width; ++xp) {
          float x = interpolate(xp, Range(0, width), Range(0, 1));
          float y = (r - cos(theta) * x) / sin(theta);
          int yp = interpolate(y, Range(0, 1), Range(0, height));
          if (yp >= 0 && yp < height) {
            for (int dx = -1; dx <= 1; dx++) {
              for (int dy = -1; dy <= 1; dy++) {
                if (xp + dx >= 0 && xp + dx < width && yp + dy >= 0 &&
                    yp + dy < height) {
                  res(xp + dx, yp + dy) = 1;
                }
              }
            }
          }
        }
      } else {
        for (int yp = 0; yp < height; ++yp) {
          float y = interpolate(yp, Range(0, height), Range(0, 1));
          float x = (r - sin(theta) * y) / cos(theta);
          int xp = interpolate(x, Range(0, 1), Range(0, width));
          if (xp >= 0 && xp < width) {
            for (int dx = -1; dx <= 1; dx++) {
              for (int dy = -1; dy <= 1; dy++) {
                if (xp + dx >= 0 && xp + dx < width && yp + dy >= 0 &&
                    yp + dy < height) {
                  res(xp + dx, yp + dy) = 1;
                }
              }
            }
          }
        }
      }
    }
  }
  progress.update(hough.width());
  progress.done();

  return res;
}

Array2D<float> threshold_arr(Array2D<float>& arr, float threshold) {
  Array2D<float> res(arr.width(), arr.height());

  Progress progress(arr.size());
  for (int i = 0; i < arr.size(); ++i) {
    if (arr(i) > threshold) {
      res(i) = 1;
    }
  }
  progress.update(arr.size());
  progress.done();

  return res;
}

bool isLocalMax(Array2D<float>& arr, int x, int y) {
  for (int dy = -absl::GetFlag(FLAGS_local_max_radius);
       dy <= absl::GetFlag(FLAGS_local_max_radius); dy++) {
    for (int dx = -absl::GetFlag(FLAGS_local_max_radius);
         dx <= absl::GetFlag(FLAGS_local_max_radius); dx++) {
      if (x + dx >= 0 && x + dx < arr.width() && y + dy >= 0 &&
          y + dy < arr.height()) {
        if (dx != 0 && dy != 0) {
          if (arr(x + dx, y + dy) >= arr(x, y)) {
            return false;
          }
        }
      }
    }
  }
  return true;
}

Array2D<float> find_local_max(Array2D<float>& arr) {
  Array2D<float> res(arr.width(), arr.height());

  Progress progress(arr.width());
  for (int y = 0; y < arr.height(); y++) {
    for (int x = 0; x < arr.width(); x++) {
      if (isLocalMax(arr, x, y)) {
        res(x, y) = arr(x, y);
      }
    }
  }
  progress.update(arr.width());
  progress.done();

  return res;
}

Image floatArrayToImage(Array2D<float>& arr,
                        const Palette& palette = Palette::RedHot()) {
  float max = arr.max();
  arr /= max;
  Image image(arr.width(), arr.height());
  for (int i = 0; i < arr.size(); ++i) {
    image(i) = palette.color(arr(i));
  }
  return image;
}

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  Array2D<float> hough(theta_res, r_res);

  std::cout << "Loading image..." << std::endl;
  Image img = Image::load(absl::GetFlag(FLAGS_image) + ".ppm");

  // farr2d lum = img.transform<float>([](Color c) { return c.luminance(); });
  // floatArrayToImage(lum).save("corner_luminance.ppm");

  // lum = threshold_arr(lum, lum.max() * 0.7);
  // floatArrayToImage(lum).save("corner_luminance_thresholded.ppm");

  std::cout << "Detecting edges..." << std::endl;
  Array2D<float> edges = detect_edges(img);

  std::cout << "Saving edges..." << std::endl;
  floatArrayToImage(edges).save(absl::GetFlag(FLAGS_image) + "_edges.ppm");

  edges = threshold_arr(edges, absl::GetFlag(FLAGS_edges_threshold));
  floatArrayToImage(edges).save(absl::GetFlag(FLAGS_image) +
                                "_edges_thresholded.ppm");

  std::cout << "Hough transforming..." << std::endl;
  hough = hough_transform(edges);

  std::cout << "Saving hough..." << std::endl;
  floatArrayToImage(hough).save(absl::GetFlag(FLAGS_image) + "_hough.ppm");

  hough = find_local_max(hough);
  std::cout << "Saving local max hough..." << std::endl;
  floatArrayToImage(hough).save(absl::GetFlag(FLAGS_image) +
                                "_hough_local_max.ppm");

  float max = hough.max();
  hough = threshold_arr(hough, max * absl::GetFlag(FLAGS_reverse_threshold));
  std::cout << "Saving thresholded hough..." << std::endl;
  floatArrayToImage(hough).save(absl::GetFlag(FLAGS_image) +
                                "_hough_thresholded.ppm");

  Array2D<float> reverse_hough(img.width(), img.height());
  reverse_hough = reverse_hough_transform(hough, img.width(), img.height());
  floatArrayToImage(reverse_hough)
      .save(absl::GetFlag(FLAGS_image) + "_reverse_hough.ppm");

  Image overlay = img;
  for (int i = 0; i < overlay.size(); ++i) {
    if (reverse_hough(i) > 0) {
      overlay(i) /= 2;
      overlay(i).r = 1;
    } else {
      float lum = overlay(i).luminance();
      overlay(i) = (overlay(i) + Color(lum, lum, lum) * 2) / 3;
    }
  }
  overlay.save(absl::GetFlag(FLAGS_image) + "_overlay.ppm");

  return EXIT_SUCCESS;
}
