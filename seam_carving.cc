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

ABSL_FLAG(std::string, image, "pagoda", "the name of the input image");
ABSL_FLAG(int, carve_x, 300, "amount to carve on the x-axis");
ABSL_FLAG(int, carve_y, 300, "amount to carve on the y-axis");
ABSL_FLAG(bool, make_movie, false, "whether to save frames for a movie");
ABSL_FLAG(bool, render_seams, false,
          "whether to save render the seams instead of carving");
ABSL_FLAG(float, rendered_seams_energy, 1.0f,
          "if rendering seams, use this as the base energy");

typedef Array2D<float> farr2d;

float energy_diff(const Image& img, int x1, int y1, int x2, int y2) {
  return vec3(img(x1, y1) - img(x2, y2)).len2();
}

float seam_energy(const Image& img, int x, int y, int dx) {
  int min_x = x - 1;
  int max_x = x + 1;
  if (min_x < 0) {
    min_x = 0;
  }
  if (max_x > img.width() - 1) {
    max_x = img.width() - 1;
  }
  if (dx == -1) {
    return energy_diff(img, min_x, y, max_x, y) +
           energy_diff(img, x, y - 1, min_x, y);
  } else if (dx == 0) {
    return energy_diff(img, min_x, y, max_x, y);
  } else {
    CHECK(dx == 1) << dx;
    return energy_diff(img, min_x, y, max_x, y) +
           energy_diff(img, x, y - 1, max_x, y);
  }
}

// float energy(const Image& img, int x, int y) {
//   Color cx, cy;
//   if (x > 0) {
//     cx = img(x - 1, y);
//   }
//   if (y > 0) {
//     cy = img(x, y - 1);
//   }
//   vec3 didx = img(x, y) - cx;
//   vec3 didy = img(x, y) - cy;
//   return didx.len2() + didy.len2();
// }

// Array2D<float> toEnergyMap(Image& image) {
//   Array2D<float> res(image.width(), image.height());
//   for (int y = 0; y < image.height(); ++y) {
//     for (int x = 0; x < image.width(); ++x) {
//       res(x, y) = energy(image, x, y);
//     }
//   }

//   return res;
// }

struct Seam {
  int x = 0, y = 0;
  float energy = -1;
  Seam* parent = 0;
};

Array2D<Seam> calculate_seams(const Image& image, const farr2d* mask = 0) {
  Array2D<Seam> res(image.shape());

  for (int x = 0; x < image.width(); ++x) {
    res(x, 0).x = x;
    res(x, 0).y = 0;
    // res(x, 0).energy = energy(image, x, 0);
    res(x, 0).energy = 3;
    if (mask) {
      res(x, 0).energy += (*mask)(x, 0);
    }
  }

  for (int y = 1; y < image.height(); ++y) {
    for (int x = 0; x < image.width(); ++x) {
      Seam* seam = &res(x, y);
      seam->x = x;
      seam->y = y;
      for (int dx = -1; dx <= 1; ++dx) {
        if (x + dx >= 0 && x + dx < image.width()) {
          // std::cout << x << " " << y << " " << dx << std::endl;
          Seam* candidate_parent = &res(x + dx, y - 1);
          float candidate_energy =
              candidate_parent->energy + seam_energy(image, x, y, dx);
          if (mask) {
            candidate_energy += (*mask)(x, y);
          }
          if (seam->energy < 0 || candidate_energy < seam->energy) {
            res(x, y).energy = candidate_energy;
            res(x, y).parent = candidate_parent;
          }
        }
      }
    }
  }

  return res;
}

Seam* getMinSeam(const Array2D<Seam>& seams) {
  return std::min_element(
      seams.last_row().begin(), seams.last_row().end(),
      [](const Seam& s1, const Seam& s2) { return s1.energy < s2.energy; });
}

void sortSeams(const Array2D<Seam>& seams,
               std::vector<const Seam*>* sorted_seams) {
  // TODO: can we do this with something like std::copy?
  for (auto i = seams.last_row().cbegin(); i != seams.last_row().cend(); ++i) {
    sorted_seams->push_back(i);
  }
  std::sort(
      sorted_seams->begin(), sorted_seams->end(),
      [](const Seam* s1, const Seam* s2) { return s1->energy < s2->energy; });
}

template <class value_type>
void colorSeam(Array2D<value_type>& img, const Seam* seam,
               const value_type& color, int width = 1, bool overwrite = false) {
  while (seam != 0) {
    // std::cout << seam->x << " " << seam->y << " " << seam->energy <<
    // std::endl;
    for (int dx = -width; dx <= width; ++dx) {
      if (img.is_safe(seam->x + dx, seam->y)) {
        if (overwrite) {
          img(seam->x + dx, seam->y) = color;
        } else {
          img(seam->x + dx, seam->y) += color;
        }
      }
    }
    seam = seam->parent;
  }
}

Image carveSeam(const Image& img, Seam* seam) {
  Image res(img.width() - 1, img.height());
  while (seam != 0) {
    for (int x = 0; x < seam->x; ++x) {
      res(x, seam->y) = img(x, seam->y);
    }
    for (int x = seam->x + 1; x < img.width(); ++x) {
      res(x - 1, seam->y) = img(x, seam->y);
    }
    seam = seam->parent;
  }
  return res;
}

Image smearSeam(const Image& img, Seam* seam) {
  Image res(img.width() + 1, img.height());
  while (seam != 0) {
    for (int x = 0; x <= seam->x; ++x) {
      res(x, seam->y) = img(x, seam->y);
    }
    res(seam->x + 1, seam->y) =
        (img(seam->x, seam->y) + img(seam->x + 1, seam->y)) / 2;
    for (int x = seam->x + 1; x < img.width(); ++x) {
      res(x + 1, seam->y) = img(x, seam->y);
    }
    seam = seam->parent;
  }
  return res;
}

int frame_num = 0;

void carve(Image& img, int amount, bool save_frames = false,
           bool render_seams = false) {
  Progress progress(amount);
  Image img2 = img;
  farr2d mask(img.shape());
  for (int i = 0; i < amount; ++i) {
    progress.update(i);
    // farr2d energy_map = toEnergyMap(img);
    // Array2D<Seam> seams = calculate_seams(energy_map);
    Array2D<Seam> seams = calculate_seams(img, &mask);
    Seam* min_seam = getMinSeam(seams);
    if (render_seams) {
      colorSeam(mask, min_seam, absl::GetFlag(FLAGS_rendered_seams_energy), 0);
    } else {
      img = carveSeam(img, min_seam);
      if (save_frames) {
        img2.clear();
        img2.blit(img, 0, 0);
        img2.save(absl::GetFlag(FLAGS_image) + "_frame_" +
                  std::to_string(frame_num++) + ".ppm");
      }
    }
  }
  progress.update(amount);
  progress.done();

  if (render_seams) {
    std::string filename = absl::GetFlag(FLAGS_image) + "_min_seams.ppm";
    std::cout << "Saving rendered seams to: " << filename << std::endl;
    Image with_seams = img;
    for (int i = 0; i < img.size(); ++i) {
      if (mask(i)) {
        with_seams(i) =
            /*with_seams(i) * 0.5 +*/ Color(1, 0, 0) * mask(i) * 1000;
      }
    }
    with_seams.save(filename);
  } else {
    img.save(absl::GetFlag(FLAGS_image) + "_carved.ppm");
  }
}

void carveHorizontal(Image& img, int amount) {
  img = img.transpose();
  carve(img, amount);
  img = img.transpose();
}

// void smear(Image& img, int amount) {
//   Progress progress(amount);
//   for (int i = 0; i < amount; ++i) {
//     progress.update(i);
//     farr2d energy_map = toEnergyMap(img);
//     Array2D<Seam> seams = calculate_seams(energy_map);
//     Seam* min_seam = getMinSeam(seams);
//     img = smearSeam(img, min_seam);
//   }
//   progress.update(amount);
//   progress.done();
// }

// void smearHorizontal(Image& img, int amount) {
//   img = img.transpose();
//   smear(img, amount);
//   img = img.transpose();
// }

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);

  Image img = Image::load(absl::GetFlag(FLAGS_image) + ".ppm");
  // farr2d energy_map = toEnergyMap(img);
  // Image::fromFloatArray(energy_map)
  //     .save(absl::GetFlag(FLAGS_image) + "_energy.ppm");

  // // std::vector<const Seam*> sorted_seams;
  // // sortSeams(seams, &sorted_seams);
  // Array2D<Seam> seams = calculate_seams(img);
  // Seam* min_seam = getMinSeam(seams);
  // std::cout << "coloring seam..." << std::endl;
  // Image img2(img);
  // colorSeam(img2, min_seam, Color(1, 0, 0), 1, true);
  // img2.save(absl::GetFlag(FLAGS_image) + "_min_seam.ppm");
  // return 0;

  if (absl::GetFlag(FLAGS_carve_x) > 0) {
    carve(img, absl::GetFlag(FLAGS_carve_x), absl::GetFlag(FLAGS_make_movie),
          absl::GetFlag(FLAGS_render_seams));
    // } else {
    //   smear(img, -absl::GetFlag(FLAGS_carve_x));
  }

  if (absl::GetFlag(FLAGS_carve_y) > 0) {
    carveHorizontal(img, absl::GetFlag(FLAGS_carve_y));
    // } else {
    //   smearHorizontal(img, -absl::GetFlag(FLAGS_carve_y));
  }
  img.save(absl::GetFlag(FLAGS_image) + "_carved2.ppm");

  return EXIT_SUCCESS;
}
