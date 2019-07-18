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
#include "sdf.h"
#include "range.h"
#include "ray.h"
#include "progress.h"
#include "light.h"
#include "rand_utils.h"
#include "scenes/scene1.h"
#include "scenes/stars1.h"
#include "scenes/cube_scene.h"

const int WIDTH = 1000;
const int HEIGHT = 1000;

const int MAX_MARCHING_STEPS = 500;
// const int MAX_MARCHING_STEPS = 500000;
const float MAX_DIST = 10000;
const float EPSILON = 0.001;

// const bool DO_SHADING = false;
const bool DO_SHADING = true;

const int AA_FACTOR = 1;
// const int AA_FACTOR = 4;

// const int REFLECTION_DEPTH = 1;
const int REFLECTION_DEPTH = 5;

const int ROUGHNESS_ITERATIONS = 1;
// const int ROUGHNESS_ITERATIONS = 5;

const bool USE_GRAVITY = false;
// const bool USE_GRAVITY = true;

const bool LIGHT_DECAY = false;
// const bool LIGHT_DECAY = true;

vec3 eye_pos(0, 0, -5);
const float SCREEN_Z = 0;

bool march(Ray& ray, const Scene& scene, SDFResult* res) {
  for (int i = 0; i < MAX_MARCHING_STEPS; ++i) {
    *res = scene.root()->sdf(ray.origin);
    if (res->dist < EPSILON) {
      return true;
    } else if (abs(res->dist) > MAX_DIST) {
      return false;
    }
    if (!USE_GRAVITY) {
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
  for (float t = EPSILON * 100; t < dist_to_light;) {
    SDFResult r = scene.root()->sdf(ray_to_light.origin + ray_to_light.direction * t);
    if (r.dist < EPSILON) return 0.0;
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
    if (DO_SHADING) {
      shade = shadow(Ray(v, norm_to_light), scene, dist_to_light);
    }
    float factor = cos_alpha * shade;
    if (LIGHT_DECAY) {
      factor = factor * 50 / (dist_to_light * dist_to_light);
    }
    if (factor < 0) factor = 0;
    total_factor += factor * diffuse_factor;
  }
  return r.material.rgb(v) * total_factor;
}

FRGB shoot(Ray ray, const Scene& scene, int remaining_depth=REFLECTION_DEPTH);

FRGB reflection(Ray ray, const Scene& scene, const Material& mat, int remaining_depth) {
  if (remaining_depth == 0) return colors::BLACK;
  vec3 normal = scene.root()->normal(ray.origin);
  ray.direction.ireflect(normal);
  int num_iters = 1;
  if (mat.roughness > 0) {
    num_iters = ROUGHNESS_ITERATIONS;
  }
  std::vector<FRGB> rgbs;
  vec3 original_dir = ray.direction;
  for (int i = 0; i < num_iters; ++i) {
    if (mat.roughness > 0) {
      vec3 noise_vec = Vec3::random() * mat.roughness;
      ray.direction = original_dir + noise_vec;
      ray.direction.inormalize();
    }
    ray.march(EPSILON * 5);
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

  Image img(WIDTH, HEIGHT);

  time_t last_update = time(0);
  time_t seconds_until_update = 60 * 5;
  Progress progress(WIDTH);
  for (int x = 0; x < WIDTH; ++x) {
    if (time(0) - last_update > seconds_until_update) {
      last_update = time(0);
      img.save("intermediate.ppm");
      system("open intermediate.ppm");
    }
    for (int y = 0; y < HEIGHT; ++y) {
      std::vector<FRGB> rgbs;
      for (int dx = 0; dx < AA_FACTOR; ++dx) {
        for (int dy = 0; dy < AA_FACTOR; ++dy) {
          float x_screen = interpolate(x * AA_FACTOR + dx, Range(0, WIDTH * AA_FACTOR), Range(-1, 1));
          float y_screen = interpolate(y * AA_FACTOR + dy, Range(0, HEIGHT * AA_FACTOR), Range(1, -1));
          Ray ray(eye_pos, vec3(x_screen, y_screen, SCREEN_Z - eye_pos.z));

          rgbs.push_back(shoot(ray, scene));
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
