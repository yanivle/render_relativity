#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <vector>
#include <fstream>
#include <iostream>
#include "palette.h"
#include "matrix.h"
#include "image.h"
#include "doublematrix.h"
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

int main(void)
{
  Renderer renderer;
  Scene scene;
  createScene1(&scene);
  // createStars1Scene(&scene);
  // createCubeScene(&scene);

  renderer.modifiable_view_world_matrix() = Mat4::view_to_world(scene.rendering_params().camera_settings.eye_pos,
                                            scene.rendering_params().camera_settings.target,
                                            scene.rendering_params().camera_settings.up);

  vec3 v(0, 0, 0);
  std::cout << "Before: " << v.str() << " after: " << (renderer.view_world_matrix() * v).str() << std::endl;
  v = vec3(0, 0, 1);
  std::cout << "Before: " << v.str() << " after: " << (renderer.view_world_matrix() * v).str() << std::endl;
  std::cout << "World transform matrix:" << std::endl;
  std::cout << renderer.view_world_matrix().str() << std::endl;

  std::cout << "Total SDFs: " << scene.objects().size() << std::endl;
  std::cout << "Total lights: " << scene.lights().size() << std::endl;
  std::cout << "Total masses: " << scene.masses().size() << std::endl;

  Image img(scene.rendering_params().width, scene.rendering_params().height);

  time_t last_update = time(0);
  time_t seconds_until_update = 60 * 5;
  Progress progress(scene.rendering_params().width);
  for (int x = 0; x < scene.rendering_params().width; ++x) {
    if (time(0) - last_update > seconds_until_update) {
      last_update = time(0);
      img.save("intermediate.ppm");
      system("open intermediate.ppm");
    }
    for (int y = 0; y < scene.rendering_params().height; ++y) {
      std::vector<Color> colors;
      for (int dx = 0; dx < scene.rendering_params().aa_factor; ++dx) {
        for (int dy = 0; dy < scene.rendering_params().aa_factor; ++dy) {
          float x_dir = interpolate(x * scene.rendering_params().aa_factor + dx, Range(0, scene.rendering_params().width * scene.rendering_params().aa_factor), Range(-1, 1));
          float y_dir = interpolate(y * scene.rendering_params().aa_factor + dy, Range(0, scene.rendering_params().height * scene.rendering_params().aa_factor), Range(1, -1));
          // float z_dir = scene.rendering_params().screen_z - scene.rendering_params().camera_settings.eye_pos.z;
          float z_dir = scene.rendering_params().screen_z;
          Ray ray(vec3(), vec3(x_dir, y_dir, z_dir).normalize());

          colors.push_back(renderer.shoot(ray, scene, scene.rendering_params().reflection_depth));
        }
      }
      img(x, y) = Color::average(colors);
    }
    progress.update(x);
  }
  progress.done();

  img.save("output.ppm");

  return EXIT_SUCCESS;
}
