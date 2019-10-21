#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <vector>
#include <fstream>
#include <iostream>
#include "palette.h"
#include "image.h"
#include "vec3.h"
#include "mat4.h"
#include "sdf.h"
#include "range.h"
#include "ray.h"
#include "progress.h"
#include "light.h"
#include "rand_utils.h"
#include "rendering_params.h"
#include "renderer.h"
#include "scenes/scenes.h"
#include "object_registry.h"
#include "fft.h"
#include "filters.h"
#include <thread>
#include <unistd.h>
#include <atomic>
#include "counters.h"

std::atomic<int> global_y = 0;
Scene* scene = 0;
Renderer renderer;

void progress_thread() {
  Progress progress(scene->rendering_params().height);
  while (global_y < scene->rendering_params().height) {
    progress.update(global_y);
    usleep(1000 * 100);
  }
  progress.update(global_y);
  progress.done();
}

DEFINE_COUNTER(rays);

void render_thread(Image* image) {
  int y = global_y++;
  int width = scene->rendering_params().width;
  int height = scene->rendering_params().height; 
  int aa_factor = scene->rendering_params().aa_factor;
  while (y < height) {
    // std::cout << "Rendering line: " << y << std::endl;
    for (int x = 0; x < width; ++x) {
      Color color;
      for (int dx = 0; dx < aa_factor; ++dx) {
        for (int dy = 0; dy < aa_factor; ++dy) {
          float x_dir = interpolate(x * aa_factor + dx, Range(0, width * aa_factor), Range(-1, 1));
          float y_dir = interpolate(y * aa_factor + dy, Range(0, height * aa_factor), Range(1, -1));
          float z_dir = scene->rendering_params().screen_z;
          Ray ray(vec3(), vec3(x_dir, y_dir, z_dir).normalize());
          COUNTER_INC(rays);

          color += renderer.shoot(ray, scene->rendering_params().reflection_depth);
        }
      }
      (*image)(x, y) = color / (aa_factor * aa_factor);
    }
    y = global_y++;
  }
}

std::string counter_filename(std::string basename, int count, std::string suffix) {
  return basename + std::to_string(count) + suffix;
}

int main(void) {
  scene = scenes::GetScene("Spheres");
  renderer.setScene(scene);

  bool apply_post_processing = false;
  bool double_image_before_convolution = true;
  bool save_snapshot = true;
  bool resume_from_snapshot = false;

  renderer.modifiable_view_world_matrix() = Mat4::view_to_world(scene->rendering_params().camera_settings.eye_pos,
                                            scene->rendering_params().camera_settings.target,
                                            scene->rendering_params().camera_settings.up);

  std::cout << "Total SDFs: " << registry::registry.numObjects() << std::endl;
  std::cout << "Total lights: " << scene->lights().size() << std::endl;
  std::cout << "Total masses: " << scene->masses().size() << std::endl;

  std::cout << "Resolution: " << scene->rendering_params().width << 'x'
                              << scene->rendering_params().height << std::endl;

  Image img(scene->rendering_params().width, scene->rendering_params().height);

  for (int frame = 0; frame < scene->rendering_params().animation_params.frames; ++frame) {
    if (resume_from_snapshot) {
      img.deserialize(counter_filename("output/render", frame, ".img"));
    } else {
      // float animation_fraction = float(frame) / scene->rendering_params().animation_params.frames;
      // vec3 eye_movement = vec3(100 * sin(-1 + M_PI * animation_fraction), 0, -100 * cos(-1 + M_PI * animation_fraction) + 100);
      vec3 eye_movement;
      renderer.modifiable_view_world_matrix() = Mat4::view_to_world(scene->rendering_params().camera_settings.eye_pos + eye_movement,
                                                scene->rendering_params().camera_settings.target,
                                                scene->rendering_params().camera_settings.up);
      global_y = 0;
      const int num_threads = std::thread::hardware_concurrency();
      std::cout << "Rendering frame " << frame << "/"
                << scene->rendering_params().animation_params.frames << " with "
                << num_threads << " threads..." << std::endl;
      std::vector<std::thread> threads;
      for (int i = 0; i < num_threads; ++i) {
        threads.push_back(std::thread(render_thread, &img));
      }
      progress_thread();
      for (std::thread& thread : threads) {
        thread.join();
      }

      if (save_snapshot) {
        img.serialize(counter_filename("output/render", frame, ".img"));
      }
    }

    if (apply_post_processing) {
      std::cout << "Applying post processing effects..." << std::endl;
      if (double_image_before_convolution) {
        img = img.resize(img.width() * 2, img.height() * 2);
        filters::Convolve(img, filters::Bloom(scene->rendering_params().width * 2, scene->rendering_params().height * 2));
        img = img.resize(img.width() / 2, img.height() / 2);
      } else {
        filters::Convolve(img, filters::Bloom(scene->rendering_params().width, scene->rendering_params().height));
      }
    }
    img.save(counter_filename("output/output", frame, ".ppm").c_str());

    std::cout << COUNTERS_STR() << std::endl;
  }

  return EXIT_SUCCESS;
}
