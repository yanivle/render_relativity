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
#include "scenes/scene1.h"
#include "scenes/stars1.h"
#include "scenes/cube_scene.h"
#include "object_registry.h"
#include "fft.h"
#include "filters.h"
#include <thread>
#include <unistd.h>
#include "world_constants.h"
#include <atomic>
#include "counters.h"

std::atomic<int> global_y = 0;
Renderer renderer;
Scene scene;

void progress_thread() {
  Progress progress(scene.rendering_params().height);
  while (global_y < scene.rendering_params().height) {
    progress.update(global_y);
    usleep(1000 * 100);
  }
  progress.update(global_y);
  progress.done();
}

DEFINE_COUNTER(rays);

void render_thread(Image* image) {
  int y = ++global_y;
  while (y < scene.rendering_params().height) {
    // std::cout << "Rendering line: " << y << std::endl;
    for (int x = 0; x < scene.rendering_params().width; ++x) {
      std::vector<Color> colors;
      for (int dx = 0; dx < scene.rendering_params().aa_factor; ++dx) {
        for (int dy = 0; dy < scene.rendering_params().aa_factor; ++dy) {
          float x_dir = interpolate(x * scene.rendering_params().aa_factor + dx, Range(0, scene.rendering_params().width * scene.rendering_params().aa_factor), Range(-1, 1));
          float y_dir = interpolate(y * scene.rendering_params().aa_factor + dy, Range(0, scene.rendering_params().height * scene.rendering_params().aa_factor), Range(1, -1));
          float z_dir = scene.rendering_params().screen_z;
          Ray ray(vec3(), vec3(x_dir, y_dir, z_dir).normalize());
          COUNTER_INC(rays);

          colors.push_back(renderer.shoot(ray, scene, scene.rendering_params().reflection_depth));
        }
      }
      (*image)(x, y) = Color::average(colors);
    }
    y = ++global_y;
  }
}

std::string counter_filename(std::string basename, int count, std::string suffix) {
  return basename + std::to_string(count) + suffix;
}

int main(void) {
  createScene1(&scene);
  // createStars2Scene(&scene);
  // createCubeScene(&scene);

  // renderer.modifiable_view_world_matrix() = Mat4::view_to_world(scene.rendering_params().camera_settings.eye_pos,
  //                                           scene.rendering_params().camera_settings.target,
  //                                           scene.rendering_params().camera_settings.up);

  std::cout << "Total SDFs: " << registry::registry.numObjects() << std::endl;
  std::cout << "Total lights: " << scene.lights().size() << std::endl;
  std::cout << "Total masses: " << scene.masses().size() << std::endl;

  std::cout << "Resolution: " << scene.rendering_params().width << 'x'
                              << scene.rendering_params().height << std::endl;

  Image img(scene.rendering_params().width, scene.rendering_params().height);

  for (int frame = 0; frame < scene.rendering_params().animation_params.frames; ++frame) {
    // float animation_fraction = float(frame) / scene.rendering_params().animation_params.frames;
    // vec3 eye_movement = vec3(100 * sin(-1 + M_PI * animation_fraction), 0, -100 * cos(-1 + M_PI * animation_fraction) + 100);
    vec3 eye_movement;
    renderer.modifiable_view_world_matrix() = Mat4::view_to_world(scene.rendering_params().camera_settings.eye_pos + eye_movement,
                                              scene.rendering_params().camera_settings.target,
                                              scene.rendering_params().camera_settings.up);

    // *(world_constants::values["blackhole_mass"]) = 5 * animation_fraction;
    // *(world_constants::values["blackhole_radius"]) = 7 * animation_fraction;
    global_y = 0;
    const int num_threads = std::thread::hardware_concurrency();
    std::cout << "Rendering frame " << frame << "/"
              << scene.rendering_params().animation_params.frames << " with "
              << num_threads << " threads..." << std::endl;
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
      threads.push_back(std::thread(render_thread, &img));
    }
    progress_thread();
    for (std::thread& thread : threads) {
      thread.join();
    }

    // img.serialize(counter_filename("output/render", frame, ".img"));
    // img.deserialize("render.img");

    // img.save(counter_filename("output/output", frame, ".ppm").c_str());

    // std::cout << "Applying post processing effects..." << std::endl;
    // filters::Convolve(img, filters::Bloom(scene.rendering_params().width, scene.rendering_params().height));
    img.save(counter_filename("output/output_bloomed", frame, ".ppm").c_str());

    world_constants::time += scene.rendering_params().animation_params.time_delta;

    std::cout << COUNTERS_STR() << std::endl;
  }

  return EXIT_SUCCESS;
}
