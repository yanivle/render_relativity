#ifndef STARS1_H
#define STARS1_H

#include "../scene.h"

void AddStar(Scene* scene) {
  Material material(colors::WHITE, rand_range(0, 1), 0, 0, 0);
  vec3 center(rand_range(-100, 100), rand_range(-100, 100), 500);
  float radius = rand_range(0.1, 0.4);
  SDF* star = scene->own(new Sphere(vec3(0, 0, 0), radius, material));
  star = scene->addObject(new Translate(star, center));
}

void AddBigStar(const FRGB& color1, const FRGB& color2, float perlin_scale,
                float ambient, float diffuse,
                float deformation_scale, float deformation_magnitude,
                float radius, const vec3& center, float mass,
                Scene* scene) {
  PerlinNoiseColorizer *colorizer = new PerlinNoiseColorizer(color1, color2, perlin_scale);
  Material material(colorizer, ambient, diffuse, 0, 0);
  SDF* star = scene->own(new Sphere(vec3(0, 0, 0), radius, material));
  colorizer->setSurface((Sphere*)star);
  if (deformation_magnitude > 0) {
    star = scene->own(new PerlinDeformation((Sphere*)star, deformation_scale, deformation_magnitude));
  }
  star = scene->addObject(new Translate(star, center));
  scene->addMass((PointMass(center, mass)));
}

void createStars1Scene(Scene *res) {
  // eye_pos = vec3(0, 0, -3);

  // Sun.
  float sun_radius = 2;
  vec3 sun_center = vec3(-2.5, -2.5, 15);
  if (false)
  AddBigStar(FRGB(0xF6, 0xF7, 0xD2), FRGB(0xF9, 0xF4, 0x8F), 1,
              0.0, 0.1,
              1, 0.1,
              // 1, 0.0001,
              sun_radius, sun_center, 0, res);

  // Red star.
  float red_radius = 1.5;
  vec3 red_center = vec3(1.5, 1.5, 20);
  if (false)
  AddBigStar(FRGB(161, 0, 0), FRGB(100, 50, 0), 15,
              // 0, 0.075,
              0, 0.2,
              0.1, 0.1,
              red_radius, red_center, 0, res);

  // Earth.
  // if (false)
  AddBigStar(FRGB(107,147,214), FRGB(216,197,150), 15,
              // 0.1, 0.001,
              0, 0.015,
              100, 0.0001,
              0.2, vec3(3, -1.5, 25), 0, res);

  // Moon.
  if (false)
  AddBigStar(FRGB(107,147,214), FRGB(216,197,150), 15,
              // 0.1, 0.001,
              1, 1.0,
              0, 0,
              0.05, vec3(3 - 0.6, -1.5, 25), 0, res);

  // Left.
  if (false)
  AddBigStar(FRGB(107,147,214), FRGB(216,197,150), 5,
              // 0, 0.0001,
              0, 0.07,
              0.5, 0.01,
            0.5, vec3(-3.3, 1, 15), 0, res);


  // Black hole.
  if (false)
  AddBigStar(colors::BLACK, colors::BLACK, 0,
              0, 0,
              0, 0,
              0.000001, vec3(-0.2, -0.2, 5), 0.01, res);

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
    res->addLight(new PointLight(red_center + vec3::random().normalize() * (red_radius + 1)));
  }
}

#endif
