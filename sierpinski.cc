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
#include "filters.h"
#include "image.h"
#include "palette.h"
#include "progress.h"
#include "rand_utils.h"
#include "range.h"
#include "vec3.h"

int width = 1024;
int height = 1024;

vec3 fixed_triangle[3] = {
  vec3(100, 100, 0),
  vec3(width - 100, 200, 0),
  vec3(width / 2, height - 100, 0)
};

vec3 skewed_triangle[8] = {
  vec3(100, 100, 0),
  vec3(100, 100, 0),
  vec3(100, 100, 0),
  vec3(100, 100, 0),
  vec3(100, 100, 0),
  vec3(100, 100, 0),
  vec3(width - 100, 200, 0),
  vec3(width / 2, height - 100, 0)
};

vec3 fixed_skewed_square[4] = {
  vec3(100, 100, 0),
  vec3(width - 100, 200, 0),
  vec3(100, height - 100, 0),
  vec3(width - 200, height - 200, 0),
};

vec3 fixed_square[4] = {
  vec3(100, 100, 0),
  vec3(width - 100, 100, 0),
  vec3(100, height - 100, 0),
  vec3(width - 100, height - 100, 0),
};

vec3 fern1[3] = {
  vec3(100, 700, 0),
  vec3(450, 800, 0),
  vec3(500, 900, 0),
};

vec3 fern2[3] = {
  vec3(900, 100, 0),
  vec3(850, 820, 0),
  vec3(500, 950, 0),
};

void render(Array2D<float>* image, int num_iters, vec3* fixed, int n_fixed, float alpha = 0.5, vec3* p0 = 0) {
  vec3 p;
  if (p0 != 0) {
    p = *p0;
  }

  Progress progress(num_iters);
  for (int i = 0; i < num_iters; ++i) {
    progress.update(i);
    vec3& q = fixed[random() % n_fixed];
    p = p * alpha + q * (1 - alpha);
    (*image)(p.x, p.y)++;
  }

  progress.update(num_iters);
  progress.done();

  if (p0 != 0) {
    *p0 = p;
  }
}

// void draw_fern(Image& img) {
//   vec3 p;
//   Array2D<float> arr(width, height);
//   for (int i = 0; i < 100; ++i) {
//     if (random() % 2 == 0) {
//       render(&arr, 100, fern1, 3, 0.5, &p);
//     } else {
//       render(&arr, 100, fern2, 3, 0.5, &p);
//     }
//   }

//   Color c(0, 1, 0);

//   Progress progress(arr.size());
//   for (int i = 0; i < arr.size(); ++i) {
//     progress.update(i);
//     if (arr(i) != 0) {
//       img(i) = c;
//     } else {
//       img(i) = Color(1, 1, 1);
//     }
//   }
//   progress.update(arr.size());
//   progress.done();

//   img.save("/tmp/fern.ppm");
// }

void draw_sierpinski(Image& img, vec3* fixed, int n_fixed, float alpha, const char* filename, int iters = 1000000) {
  Array2D<float> arr(width, height);
  render(&arr, iters, fixed, n_fixed, alpha);

  Color c(0, 0, 0);

  Progress progress(arr.size());
  for (int i = 0; i < arr.size(); ++i) {
    progress.update(i);
    if (arr(i) != 0) {
      img(i) = c;
    }
  }
  progress.update(arr.size());
  progress.done();

  img.save(filename);
}

Image draw_starting_points(vec3* fixed, int n_fixed) {
  Color c(1, 1, 1);
  c *= 5000;
  
  Image image(width, height);
  for (int i = 0; i < n_fixed; i++) {
    vec3& p = fixed[i];
    (image)(p.x, p.y) = c;
  }

  image = std::move(image.resize(width * 2, height * 2));
  filters::Convolve(image, filters::Bloom(width * 2, height * 2));
  image = std::move(image.resize(width, height));

  for (int i = 0; i < image.size(); ++i) {
    image(i) = Color(1, 1, 1) - Color(RGB(image(i)));
  }

  image.save("/tmp/start_triangle.ppm");

  return image;
}

int main(int argc, char** argv) {
  Image img(width, height);

  // img = draw_starting_points(fixed_triangle, 3);
  // img.serialize("/tmp/starting_points1.img");
  // img.deserialize("/tmp/starting_points1.img");
  // draw_sierpinski(img, fixed_triangle, 3, 0.5, "/tmp/sierpinski.ppm");

  // img = draw_starting_points(skewed_triangle, 8);
  // img.serialize("/tmp/starting_points2.img");
  // img.deserialize("/tmp/starting_points2.img");
  // draw_sierpinski(img, skewed_triangle, 8, 0.5, "/tmp/sierpinski_skewed.ppm");

  // img.deserialize("/tmp/starting_points1.img");
  // draw_sierpinski(img, fixed_triangle, 3, 0.45, "/tmp/sierpinski_45.ppm");

  // img.deserialize("/tmp/starting_points1.img");
  // draw_sierpinski(img, fixed_triangle, 3, 0.55, "/tmp/sierpinski_55.ppm");

  // img = draw_starting_points(fixed_skewed_square, 4);
  // draw_sierpinski(img, fixed_skewed_square, 4, 0.33, "/tmp/sierpinski_square_skewed_33.ppm");

  // img = draw_starting_points(fixed_square, 4);
  // draw_sierpinski(img, fixed_square, 4, 0.5, "/tmp/sierpinski_square_05.ppm");

  // img = draw_starting_points(fixed_square, 4);
  // draw_sierpinski(img, fixed_square, 4, 0.33, "/tmp/sierpinski_square_33.ppm");

  // const int num_verts = 20;
  // vec3 *vecs = new vec3[num_verts];

  // for (int i = 0; i < num_verts; ++i) {
  //   vecs[i] = vec3(rand_range(100, width-100), rand_range(100, height-100), 0);
  // }

  // img = draw_starting_points(vecs, num_verts);
  // draw_sierpinski(img, vecs, num_verts, 0.3, "/tmp/sierpinski_poly.ppm");

  // delete []vecs;

  draw_fern(img);

  return EXIT_SUCCESS;
}
