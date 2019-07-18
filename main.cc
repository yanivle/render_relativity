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
#include "point_mass.h"
#include "rand_utils.h"

const int WIDTH = 1000;
const int HEIGHT = 1000;

// const int MAX_MARCHING_STEPS = 500;
const int MAX_MARCHING_STEPS = 500000;
const float MAX_DIST = 10000;
const float EPSILON = 0.001;

// const bool DO_SHADING = false;
const bool DO_SHADING = true;

const int AA_FACTOR = 1;
// const int AA_FACTOR = 4;

const int REFLECTION_DEPTH = 1;
// const int REFLECTION_DEPTH = 5;

const int ROUGHNESS_ITERATIONS = 1;
// const int ROUGHNESS_ITERATIONS = 5;

// const bool USE_GRAVITY = false;
const bool USE_GRAVITY = true;

// const bool LIGHT_DECAY = false;
const bool LIGHT_DECAY = true;

std::vector<PointMass> masses;

vec3 eye_pos(0, 0, -5);
std::vector<Light*> lights;
const float SCREEN_Z = 0;

SDF *scene = 0;

void Add(SDF* sdf) {
  if (scene == 0) {
    scene = sdf;
  } else {
    scene = new Union(scene, sdf);
  }
}

bool march(Ray& ray, SDFResult* res) {
  for (int i = 0; i < MAX_MARCHING_STEPS; ++i) {
    *res = scene->sdf(ray.origin);
    if (res->dist < EPSILON) {
      return true;
    } else if (abs(res->dist) > MAX_DIST) {
      return false;
    }
    if (!USE_GRAVITY) {
      ray.march(res->dist);
    } else {
      ray.marchWithGravity(res->dist, masses);
    }
  }
  return false;
}

float shadow(Ray ray_to_light, float dist_to_light) {
  float res = 1.0;
  const float k = 16;
  for (float t = EPSILON * 100; t < dist_to_light;) {
    SDFResult r = scene->sdf(ray_to_light.origin + ray_to_light.direction * t);
    if (r.dist < EPSILON) return 0.0;
    res = fmin(res, k * r.dist / t);
    t += r.dist;
  }
  return res;
}

FRGB diffuse(const vec3& v, const SDFResult& r, float diffuse_factor) {
  vec3 normal = scene->normal(v);
  FRGB color;
  float total_factor = 0;
  for (int i = 0; i < lights.size(); ++i) {
    vec3 norm_to_light;
    float dist_to_light;
    float cos_alpha;
    lights[i]->diffuse(v, normal, &norm_to_light, &dist_to_light, &cos_alpha);
    float shade = 1.0;
    if (DO_SHADING) {
      shade = shadow(Ray(v, norm_to_light), dist_to_light);
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

FRGB shoot(Ray ray, int remaining_depth=REFLECTION_DEPTH);

FRGB reflection(Ray ray, const Material& mat, int remaining_depth) {
  if (remaining_depth == 0) return colors::BLACK;
  vec3 normal = scene->normal(ray.origin);
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
    rgbs.push_back(shoot(ray, remaining_depth));
  }
  return FRGB::average(rgbs);
}

FRGB shoot(Ray ray, int remaining_depth) {
  SDFResult r;
  if(march(ray, &r)) {
    FRGB ambient_color;
    if (r.material.ambient > 0) {
      ambient_color = r.material.rgb(ray.origin) * r.material.ambient;
    }
    FRGB diffuse_color;
    if (r.material.diffuse > 0) {
      diffuse_color = diffuse(ray.origin, r, r.material.diffuse);
    }
    FRGB reflection_color;
    if (r.material.reflect > 0) {
      reflection_color = reflection(ray, r.material, remaining_depth - 1);
    }
    FRGB color = ambient_color + diffuse_color + reflection_color * r.material.reflect;
    return color;
  } else {
    return colors::BLACK;
  }
}

void createScene1() {
  SDF* s1 = new Sphere(vec3(0, 0, 0), 1.5, Material(colors::RED, 0.5, 1.0, 0.5, 0));
  SDF* s2 = new Sphere(vec3(0, 0, 0), 1.5, Material(colors::YELLOW, 0.5, 1.0, 0.5, 0));
  SDF* s3 = new Sphere(vec3(0, 0, 0), 1.5, Material(colors::GREEN, 0.1, 1.0, 0.5, 0));
  SDF* s4 = new Sphere(vec3(0, 0, 0), 1.5, Material(colors::BLUE, 0.1, 1.0, 0.5, 0));

  // s1 = new Periodic(s1, vec3(4, 0, 4));
  s1 = new Translate(s1, vec3(-1.7, 2, 16));
  masses.push_back(PointMass(vec3(-1.7, 2, 16), 0.001));
  //
  // s2 = new Periodic(s2, vec3(4, 0, 4));
  s2 = new Translate(s2, vec3(-1.7, 0, 16));
  masses.push_back(PointMass(vec3(-1.7, 0, 16), 0.001));
  //
  // s3 = new Periodic(s3, vec3(4, 0, 4));
  s3 = new Translate(s3, vec3(1.7, 2, 16));
  masses.push_back(PointMass(vec3(1.7, 2, 16), 0.001));
  //
  // s4 = new Periodic(s4, vec3(4, 0, 4));
  s4 = new Translate(s4, vec3(1.7, 0, 16));
  masses.push_back(PointMass(vec3(1.7, 0, 16), 0.001));

  s1 = new Smooth(s1, s2, 5);
  Add(s1);
  s3 = new Smooth(s3, s4, 5);
  Add(s3);

  vec3 plane_normal = vec3(0, 1, -0).normalize();
  vec3 cb1 = plane_normal.randomOrthonormalVec();
  vec3 cb2 = plane_normal.cross(cb1);
  std::cout << plane_normal.str() << " " << cb1.str() << " " << cb2.str() << std::endl;
  CheckerboardColorizer *floor_colorizer = new CheckerboardColorizer(colors::WHITE, colors::BLACK, 0.3);
  Material floor_mat(floor_colorizer, 0.1, 1.0, 0.9);
  SDF* floor_plane = new Plane(plane_normal, cb1, cb2, floor_mat);
  floor_colorizer->setSurface((Plane*)floor_plane);
  floor_plane = new Translate(floor_plane, vec3(0, -2, 20));
  Add(floor_plane);

  plane_normal = vec3(0, -0, -1).normalize();
  cb1 = plane_normal.randomOrthonormalVec();
  cb2 = plane_normal.cross(cb1);
  std::cout << plane_normal.str() << " " << cb1.str() << " " << cb2.str() << std::endl;
  CheckerboardColorizer *wall_colorizer = new CheckerboardColorizer(colors::WHITE, colors::BLACK, 0.3);
  Material wall_mat(wall_colorizer, 0.1, 1.0, 0.9);
  SDF* wall = new Plane(plane_normal, cb1, cb2, wall_mat);
  wall_colorizer->setSurface((Plane*)wall);
  wall = new Translate(wall, vec3(0, 0, 20));
  // wall = new Smooth(wall, floor_plane, 3);
  // wall = new Translate(wall, vec3(0, 0, 20));
  Add(wall);

  lights.push_back(new SpotLight(vec3(3, 10, -10), vec3(-0.25, -0.7, 2), M_PI/14));
}

void sphericalToCartesian(float r, float theta, float gamma, vec3* res) {
  res->x = r * sin(theta) * cos(gamma);
  res->y = r * sin(theta) * sin(gamma);
  res->z = r * cos(theta);
}

void AddStar() {
  Material material(colors::WHITE, rand_range(0, 1), 0, 0, 0);
  vec3 center(rand_range(-100, 100), rand_range(-100, 100), 500);
  float radius = rand_range(0.1, 0.4);
  SDF* star = new Sphere(vec3(0, 0, 0), radius, material);
  star = new Translate(star, center);
  Add(star);
}

void AddBigStar(const FRGB& color1, const FRGB& color2, float perlin_scale,
                float ambient, float diffuse,
                float deformation_scale, float deformation_magnitude,
                float radius, const vec3& center, float mass) {
  PerlinNoiseColorizer *colorizer = new PerlinNoiseColorizer(color1, color2, perlin_scale);
  Material material(colorizer, ambient, diffuse, 0, 0);
  SDF* star = new Sphere(vec3(0, 0, 0), radius, material);
  colorizer->setSurface((Sphere*)star);
  if (deformation_magnitude > 0) {
    star = new PerlinDeformation((Sphere*)star, deformation_scale, deformation_magnitude);
  }
  star = new Translate(star, center);
  Add(star);
  masses.push_back(PointMass(center, mass));
}

void createScene2() {
  eye_pos = vec3(0, 0, -3);

  // Sun.
  float sun_radius = 2;
  vec3 sun_center = vec3(-2.5, -2.5, 15);
  if (false)
  AddBigStar(FRGB(0xF6, 0xF7, 0xD2), FRGB(0xF9, 0xF4, 0x8F), 1,
              0.0, 0.1,
              1, 0.1,
              // 1, 0.0001,
              sun_radius, sun_center, 0);

  // Red star.
  float red_radius = 1.5;
  vec3 red_center = vec3(1.5, 1.5, 20);
  if (false)
  AddBigStar(FRGB(161, 0, 0), FRGB(100, 50, 0), 15,
              // 0, 0.075,
              0, 0.2,
              0.1, 0.1,
              red_radius, red_center, 0);

  // Earth.
  // if (false)
  AddBigStar(FRGB(107,147,214), FRGB(216,197,150), 15,
              // 0.1, 0.001,
              0, 0.015,
              100, 0.0001,
              0.2, vec3(3, -1.5, 25), 0);

  // Moon.
  if (false)
  AddBigStar(FRGB(107,147,214), FRGB(216,197,150), 15,
              // 0.1, 0.001,
              1, 1.0,
              0, 0,
              0.05, vec3(3 - 0.6, -1.5, 25), 0);

  // Left.
  if (false)
  AddBigStar(FRGB(107,147,214), FRGB(216,197,150), 5,
              // 0, 0.0001,
              0, 0.07,
              0.5, 0.01,
            0.5, vec3(-3.3, 1, 15), 0);


  // Black hole.
  if (false)
  AddBigStar(colors::BLACK, colors::BLACK, 0,
              0, 0,
              0, 0,
              0.000001, vec3(-0.2, -0.2, 5), 0.01);

  // const int NUM_BACKGROUND_STARS = 1000;
  // for (int i = 0; i < NUM_BACKGROUND_STARS; ++i) {
  //   AddStar();
  // }
  //
  // vec3 plane_normal = vec3(0, 0, -1).normalize();
  // vec3 cb1 = plane_normal.randomOrthonormalVec();
  // vec3 cb2 = plane_normal.cross(cb1);
  // std::cout << plane_normal.str() << " " << cb1.str() << " " << cb2.str() << std::endl;
  // PerlinNoiseColorizer *wall_colorizer = new PerlinNoiseColorizer(FRGB(0, 0, 30), FRGB(0, 0, 0), 100);
  // Material wall_mat(wall_colorizer, 1.0, 0, 0);
  // SDF* wall = new Plane(plane_normal, cb1, cb2, wall_mat);
  // wall_colorizer->setSurface((Plane*)wall);
  // wall = new Translate(wall, vec3(0, 0, 2000));
  // Add(wall);

  for (int i = 0; i < 50; ++i) {
    lights.push_back(new PointLight(red_center + vec3::random().normalize() * (red_radius + 1)));
  }
}

SDF* createCube(const FRGB& color = colors::RED) {
  Material material(color, 0, 0.5, 0.05);
  std::vector<vec3> normals = {
    vec3(-1, 0, 0),
    vec3(1, 0, 0),
    vec3(0, -1, 0),
    vec3(0, 1, 0),
    vec3(0, 0, -1),
    vec3(0, 0, 1),
  };

  vec3 dummy;

  std::vector<SDF*> sides;
  for (int i = 0; i < 6; ++i) {
    SDF* side = new Plane(normals[i], dummy, dummy, material);
    side = new Translate(side, normals[i]);
    sides.push_back(side);
  }
  SDF* cube = sides[0];
  for (int i = 1; i < 6; ++i) {
    cube = new Intersection(cube, sides[i]);
  }

  return cube;
}

void addCube() {
  SDF* cube = createCube();

  // chop off numbers
  SDF* sphere1 = new Sphere(vec3(0, 0, 0), 1.5, Material());
  cube = new Difference(cube, sphere1);

  // Rounded corners.
  // cube = new Expand(cube, 0.1);

  cube = new Scale(cube, 2);

  // Move to a better position.
  cube = new Translate(cube, vec3(-1.5, 1.5, 15));

  Add(cube);
}

void addChoppedSphere() {
  SDF* cube = createCube(colors::RED);
  cube = new Scale(cube, 1.2);

  SDF* sphere = new Sphere(vec3(0, 0, 0), 1.5, Material(colors::RED, 0, 0.5, 0.05));
  sphere = new Intersection(sphere, cube);


  // Move to a better position.
  sphere = new Translate(sphere, vec3(1.5, -1.2, 8));

  Add(sphere);
}

void addCheckeredFloor() {
  vec3 normal = vec3(0, 1, 0);
  vec3 cb1 = normal.randomOrthonormalVec();
  vec3 cb2 = normal.cross(cb1);
  CheckerboardColorizer *colorizer = new CheckerboardColorizer(FRGB(255, 165, 0), colors::BLACK, 0.3);
  Material material(colorizer, 0.1, 1.0, 0.5);
  SDF* floor = new Plane(normal, cb1, cb2, material);
  colorizer->setSurface((Plane*)floor);
  floor = new Translate(floor, vec3(0, -2, 20));
  Add(floor);
}

void addCheckeredWall() {
  vec3 normal = vec3(0, 0, -1);
  vec3 cb1 = normal.randomOrthonormalVec();
  vec3 cb2 = normal.cross(cb1);
  CheckerboardColorizer *colorizer = new CheckerboardColorizer(colors::BLUE, colors::BLACK, 1);
  Material material(colorizer, 0, 0.5, 0.5);
  SDF* wall = new Plane(normal, cb1, cb2, material);
  colorizer->setSurface((Plane*)wall);
  wall = new Translate(wall, vec3(0, 0, 20));
  Add(wall);
}

void createCubeScene() {
  addCube();
  addChoppedSphere();
  addCheckeredFloor();
  addCheckeredWall();
  // lights.push_back(new PointLight(vec3(10, 20, -10)));
  lights.push_back(new SpotLight(vec3(-1.5, 1.5, 0), vec3(0, 0, 1), M_PI/10));
  lights.push_back(new SpotLight(vec3(1.5, -1.2, 0), vec3(0, 0, 1), M_PI/10));
  // lights.push_back(new SpotLight(vec3(10, 20, -10), vec3(-10, -20, 10), M_PI/10));

  // lights.push_back(new PointLight(vec3(0, 0, -10)));
  lights.push_back(new PointLight(vec3(10, 20, -10)));
}

int main(void)
{
  // createScene1();
  createScene2();
  // createCubeScene();

  std::cout << "Total lights: " << lights.size() << std::endl;

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

          rgbs.push_back(shoot(ray));
        }
      }
      img(x, y) = FRGB::average(rgbs);
    }
    progress.update(x);
  }
  progress.done();

  img.save("output.ppm");

  delete scene;

  return EXIT_SUCCESS;
}
