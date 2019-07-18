#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <vector>
#include <fstream>
#include <iostream>
#include "palette.h"
#include "rgb.h"
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
#include "scenes/scene1.h"
#include "scenes/stars1.h"
#include "scenes/cube_scene.h"

vec3 eye_pos(0, 0, -5);

bool march(Ray& ray, const Scene& scene, SDFResult* res) {
  for (int i = 0; i < scene.rendering_params().max_marching_steps; ++i) {
    *res = scene.root()->sdf(ray.origin);
    if (res->dist < scene.rendering_params().epsilon) {
      return true;
    } else if (abs(res->dist) > scene.rendering_params().max_dist) {
      return false;
    }
    if (!scene.rendering_params().use_gravity) {
      ray.march(res->dist);
    } else {
      ray.marchWithGravity(res->dist, scene.masses());
    }
  }
  return false;
}

float shadow(Ray ray_to_light, const Scene& scene, float dist_to_light) {
  float res = 1.0;
  const float k = 16;
  for (float t = scene.rendering_params().epsilon * 100; t < dist_to_light;) {
    SDFResult r = scene.root()->sdf(ray_to_light.origin + ray_to_light.direction * t);
    if (r.dist < scene.rendering_params().epsilon) return 0.0;
    res = fmin(res, k * r.dist / t);
    t += r.dist;
  }
  return res;
}

FRGB diffuse(const vec3& v, const SDFResult& r, const Scene& scene, float diffuse_factor) {
  vec3 normal = scene.root()->normal(v);
  FRGB color;
  float total_factor = 0;
  for (int i = 0; i < scene.lights().size(); ++i) {
    vec3 norm_to_light;
    float dist_to_light;
    float cos_alpha;
    scene.lights()[i]->diffuse(v, normal, &norm_to_light, &dist_to_light, &cos_alpha);
    float shade = 1.0;
    if (scene.rendering_params().do_shading) {
      shade = shadow(Ray(v, norm_to_light), scene, dist_to_light);
    }
    float factor = cos_alpha * shade;
    if (scene.rendering_params().light_decay) {
      factor = factor * 50 / (dist_to_light * dist_to_light);
    }
    if (factor < 0) factor = 0;
    total_factor += factor * diffuse_factor;
  }
  return r.material.rgb(v) * total_factor;
}

FRGB shoot(Ray ray, const Scene& scene, int remaining_depth);

FRGB reflection(Ray ray, const Scene& scene, const Material& mat, int remaining_depth) {
  if (remaining_depth == 0) return colors::BLACK;
  vec3 normal = scene.root()->normal(ray.origin);
  ray.direction.ireflect(normal);
  int num_iters = 1;
  if (mat.roughness > 0) {
    num_iters = scene.rendering_params().roughness_iterations;
  }
  std::vector<FRGB> rgbs;
  vec3 original_dir = ray.direction;
  for (int i = 0; i < num_iters; ++i) {
    if (mat.roughness > 0) {
      vec3 noise_vec = Vec3::random() * mat.roughness;
      ray.direction = original_dir + noise_vec;
      ray.direction.inormalize();
    }
    ray.march(scene.rendering_params().epsilon * 5);
    rgbs.push_back(shoot(ray, scene, remaining_depth));
  }
  return FRGB::average(rgbs);
}

FRGB shoot(Ray ray, const Scene& scene, int remaining_depth) {
  SDFResult r;
  if(march(ray, scene, &r)) {
    FRGB ambient_color;
    if (r.material.ambient > 0) {
      ambient_color = r.material.rgb(ray.origin) * r.material.ambient;
    }
    FRGB diffuse_color;
    if (r.material.diffuse > 0) {
      diffuse_color = diffuse(ray.origin, r, scene, r.material.diffuse);
    }
    FRGB reflection_color;
    if (r.material.reflect > 0) {
      reflection_color = reflection(ray, scene, r.material, remaining_depth - 1);
    }
    FRGB color = ambient_color + diffuse_color + reflection_color * r.material.reflect;
    return color;
  } else {
    return colors::BLACK;
  }
}

int main(void)
{
  Scene scene;
  // createScene1(&scene);
  // createStars1Scene(&scene);
  createCubeScene(&scene);

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
      std::vector<FRGB> rgbs;
      for (int dx = 0; dx < scene.rendering_params().aa_factor; ++dx) {
        for (int dy = 0; dy < scene.rendering_params().aa_factor; ++dy) {
          float x_screen = interpolate(x * scene.rendering_params().aa_factor + dx, Range(0, scene.rendering_params().width * scene.rendering_params().aa_factor), Range(-1, 1));
          float y_screen = interpolate(y * scene.rendering_params().aa_factor + dy, Range(0, scene.rendering_params().height * scene.rendering_params().aa_factor), Range(1, -1));
          Ray ray(eye_pos, vec3(x_screen, y_screen, scene.rendering_params().screen_z - eye_pos.z));

          rgbs.push_back(shoot(ray, scene, scene.rendering_params().reflection_depth));
        }
      }
      img(x, y) = FRGB::average(rgbs);
    }
    progress.update(x);
  }
  progress.done();

  img.save("output.ppm");

  return EXIT_SUCCESS;
}
