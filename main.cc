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

std::atomic<int> global_y = 0;
Renderer renderer;
Scene scene;

void progress_thread() {
  Progress progress(scene.rendering_params().height);
  while (global_y < scene.rendering_params().height) {
    progress.update(global_y);
    usleep(1000 * 100);
  }
  progress.done();
}

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

          colors.push_back(renderer.shoot(ray, scene, scene.rendering_params().reflection_depth));
        }
      }
      (*image)(x, y) = Color::average(colors);
    }
    y = ++global_y;
  }
}

int main(void) {
  // createScene1(&scene);
  createStars2Scene(&scene);
  // createCubeScene(&scene);

  renderer.modifiable_view_world_matrix() = Mat4::view_to_world(scene.rendering_params().camera_settings.eye_pos,
                                            scene.rendering_params().camera_settings.target,
                                            scene.rendering_params().camera_settings.up);

  std::cout << "Total SDFs: " << registry::registry.numObjects() << std::endl;
  std::cout << "Total lights: " << scene.lights().size() << std::endl;
  std::cout << "Total masses: " << scene.masses().size() << std::endl;

  std::cout << "Resolution: " << scene.rendering_params().width << 'x'
                              << scene.rendering_params().height << std::endl;

  Image img(scene.rendering_params().width, scene.rendering_params().height);

  std::vector<std::thread> threads;
  const int num_threads = std::thread::hardware_concurrency();
  std::cout << "Starting " << num_threads << " threads" << std::endl;
  for (int i = 0; i < num_threads; ++i) {
    threads.push_back(std::thread(render_thread, &img));
    // std::cout << "Starting thread " << i << std::endl;
  }
  progress_thread();
  for (std::thread& thread : threads) {
    // std::cout << "Joining thread " << thread.get_id() << std::endl;
    thread.join();
  }

  img.serialize("render.img");
  // img.deserialize("render.img");

  img.save("output.ppm");

  std::cout << "Applying post processing effects..." << std::endl;
  filters::Convolve(img, filters::Bloom(scene.rendering_params().width, scene.rendering_params().height));
  img.save("output_bloomed.ppm");

  return EXIT_SUCCESS;
}
