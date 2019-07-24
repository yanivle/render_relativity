#ifndef STARS1_H
#define STARS1_H

#include "../scene.h"

void AddStar(SDF** container, Scene* scene) {
  Material material(colors::WHITE, rand_range(0.5, 1), 0, 0, 0);
  vec3 center(rand_range(-100, 100), rand_range(-100, 100), 500);
  center = center.normalize() * 1200;
  float radius = rand_range(0.3, 0.8);
  SDF* star = scene->own(new Sphere(center, radius, material));
  if (*container == 0) {
    *container = star;
  } else {
    *container = scene->own(new Union(*container, star));
  }
}

void AddStarMultiUnion(MultiUnion* container, Scene* scene) {
  Material material(colors::WHITE, rand_range(0.5, 1), 0, 0, 0);
  vec3 center(rand_range(-100, 100), rand_range(-100, 100), 500);
  center = center.normalize() * 1200;
  float radius = rand_range(0.3, 0.8);
  SDF* star = scene->own(new Sphere(center, radius, material));
  container->addChild(star);
}

void AddBigStar(const Color& color1, const Color& color2, float perlin_scale,
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
  if (mass > 0) {
    scene->addMass((PointMass(center, mass)));
  }
}

void createStars1Scene(Scene *res) {
  // Sun.
  AddBigStar(Color(255, 255, 0), Color(255, 255, 255), 1, 0.0, 1.0, 2, 20, 20+25, vec3(-25, 0, 0), 0, res);

  // Mercuri.
  AddBigStar(Color(161, 0, 0), Color(100, 50, 0), 15, 0.0, 0.01, 0.1, 0.1, 0.2, vec3(20 + 2, 0, 0), 0, res);

  // Venus.
  AddBigStar(Color(161, 0, 0), Color(100, 50, 0), 15, 0.0, 0.01, 0.1, 0.1, 0.4, vec3(20 + 4, 0, 0), 0, res);

  // Earth.
  AddBigStar(Color(79, 76, 176), Color(106, 147, 214), 15, 0.0, 0.01, 0.1, 0.1, 0.4, vec3(20 + 6, 0, 0), 0, res);

  // Mars.
  AddBigStar(Color(161, 0, 0), Color(100, 50, 0), 15, 0.0, 0.01, 0.1, 0.1, 0.1, vec3(20 + 8, 0, 0), 0, res);

  // Jupiter.
  AddBigStar(Color(0xF6, 0xF7, 0xD2), Color(0xF9, 0xF4, 0x8F), 15, 0.0, 0.01, 0.1, 0.1, 1, vec3(20 + 10, 0, 0), 0, res);

  // Saturn.
  AddBigStar(Color(161, 0, 0), Color(100, 50, 0), 15, 0.0, 0.01, 0.1, 0.1, 0.8, vec3(20 + 14, 0, 0), 0, res);

  // Uranus.
  AddBigStar(Color(79, 76, 176), Color(106, 147, 214), 15, 0.0, 0.01, 0.1, 0.1, 0.5, vec3(20 + 18, 0, 0), 0, res);

  // Neptune.
  AddBigStar(Color(79, 76, 176), Color(106, 147, 214), 15, 0.0, 0.01, 0.1, 0.1, 0.5, vec3(20 + 20, 0, 0), 0, res);

  // Black hole.
  AddBigStar(Color(0, 0, 0), Color(0, 0, 0), 0, 0.0, 0.0, 0, 0, 0.1, vec3(20 + 8, 0, 0), 1, res);

  SDF* bound_obj = res->own(new Sphere(vec3(), 1000, Material()));
  bound_obj = res->own(new Negate(bound_obj));
  SDF* stars_container = 0;

  const int NUM_BACKGROUND_STARS = 5000;
  for (int i = 0; i < NUM_BACKGROUND_STARS; ++i) {
    AddStar(&stars_container, res);
  }

  if (stars_container != 0) {
    res->addObject(new Bound(stars_container, bound_obj, 10));
  }
  
  // vec3 plane_normal = vec3(0, 0, -1).normalize();
  // vec3 cb1 = plane_normal.randomOrthonormalVec();
  // vec3 cb2 = plane_normal.cross(cb1);
  // std::cout << plane_normal.str() << " " << cb1.str() << " " << cb2.str() << std::endl;
  // PerlinNoiseColorizer *wall_colorizer = new PerlinNoiseColorizer(Color(0, 0, 30), Color(0, 0, 0), 100);
  // Material wall_mat(wall_colorizer, 1.0, 0, 0);
  // SDF* wall = new Plane(plane_normal, cb1, cb2, wall_mat);
  // wall_colorizer->setSurface((Plane*)wall);
  // wall = new Translate(wall, vec3(0, 0, 2000));
  // Add(wall);

  for (int i = 0; i < 500; ++i) {
    res->addLight(new PointLight(vec3(-25, 0, 0) + vec3::random().normalize() * 47));
  }
  // res->addLight(new PointLight(vec3(100, 0, 0)));
  // res->addLight(new PointLight(vec3(100, 10, 0)));
  // res->addLight(new PointLight(vec3(100, 20, 0)));
  // res->addLight(new PointLight(vec3(100, 30, 0)));

  res->modifiable_rendering_params().camera_settings.eye_pos = vec3(26, 0, -80);
  res->modifiable_rendering_params().camera_settings.target = vec3(26, 0, 0);

  res->modifiable_rendering_params().use_gravity = true;
}

void createStars2Scene(Scene *res) {
  // Sun.
  vec3 sun_center(0, 0, 200);
  float sun_radius = 45;
  AddBigStar(Color(255, 255, 0), Color(255, 255, 255), 1, 20.0, 0.0, 2, 20, sun_radius, sun_center, 0, res);

  // // Earth.
  AddBigStar(Color(79, 76, 176), Color(216, 197, 150), 1.3, 0.1, 0.5, 0.1, 0.3, 8, vec3(20, -15, 5), 0, res);

  // // Jupiter.
  AddBigStar(Color(255, 0, 0), Color(100, 100, 100), 3, 0.1, 1.0, 0.1, 0.2, 5, vec3(-30, -30, 45), 0, res);

  // // Saturn.
  // AddBigStar(Color(161, 0, 0), Color(100, 50, 0), 15, 0.0, 0.01, 0.1, 0.1, 0.8, vec3(-15, -10, 50), 0, res);

  // // Uranus.
  // AddBigStar(Color(79, 76, 176), Color(106, 147, 214), 15, 0.0, 0.01, 0.1, 0.1, 0.5, vec3(20, 0, 30), 0, res);

  // Black hole.
  AddBigStar(Color(0, 0, 0), Color(0, 0, 0), 0, 0.0, 0.0, 0, 0, 1, vec3(-20, -20, 10), 5, res);

  SDF* bound_obj = res->own(new Sphere(vec3(), 1000, Material()));
  bound_obj = res->own(new Negate(bound_obj));
  MultiUnion* stars_container = (MultiUnion*)res->own(new MultiUnion());
  res->addObject(new Bound(stars_container, bound_obj, 10));
  
  const int NUM_BACKGROUND_STARS = 2000;
  for (int i = 0; i < NUM_BACKGROUND_STARS; ++i) {
    AddStarMultiUnion(stars_container, res);
  }

  for (int i = 0; i < 500; ++i) {
    res->addLight(new PointLight(sun_center + vec3::random().normalize() * (sun_radius + 2)));
  }
  
  res->modifiable_rendering_params().camera_settings.eye_pos = vec3(0, 0, -200);
  res->modifiable_rendering_params().camera_settings.target = vec3(0, 0, 0);
  res->modifiable_rendering_params().do_shading = false;
  res->modifiable_rendering_params().max_marching_steps = 50000;

  res->addLight(new DirectionalLight(vec3(-1, -1, -1)));

  res->modifiable_rendering_params().use_gravity = true;
}

#endif
