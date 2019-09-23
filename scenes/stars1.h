#ifndef STARS1_H
#define STARS1_H

#include "../scene.h"
#include "../world_constants.h"

void AddStar(SDF** container, Scene* scene) {
  Material material(colors::WHITE, rand_range(0.5, 1), 0, 0, 0);
  vec3 center(rand_range(-100, 100), rand_range(-100, 100), 500);
  center = center.normalize() * 1200;
  float radius = rand_range(0.3, 0.8);
  SDF* star = new Sphere(center, radius, material);
  if (*container == 0) {
    *container = star;
  } else {
    *container = new Union(*container, star);
  }
}

void AddStarMultiUnion(MultiUnion* container, Scene* scene) {
  Color color = colors::WHITE;
  switch (rand() % 10) {
  case 0:
    color = colors::ORANGE;
    break;
  case 1:
    color = colors::CORAL;
    break;
  }
  float brightness = 0.5;
  while (rand() % 2 == 0 && brightness < 128) {
    brightness *= 2;
  }
  Material material(color, brightness, 0, 0, 0);
  vec3 center = vec3::random() * 1200;
  float radius = rand_range(0.3, 0.8);
  SDF* star = new Sphere(center, radius, material);
  container->addChild(star);
}

void AddBigStar(std::initializer_list<Color> colors, float perlin_scale,
                float ambient, float diffuse,
                float deformation_scale, float deformation_magnitude,
                float radius, const vec3& center, float mass,
                Scene* scene, char const* name_mass = 0, char const* radius_name = 0) {
  PerlinNoiseColorizer *colorizer = new PerlinNoiseColorizer(colors, perlin_scale);
  Material material(colorizer, ambient, diffuse, 0, 0);
  Sphere* sphere = new Sphere(center, radius, material);
  if (radius_name != 0) {
    world_constants::values[radius_name] = &(sphere->radius);
  }
  SDF* star = sphere;
  colorizer->setSurface((Sphere*)star);
  if (deformation_magnitude > 0) {
    star = new PerlinDeformation((Sphere*)star, deformation_scale, deformation_magnitude);
    SDF* bound_obj = new Sphere(center, radius + deformation_magnitude, Material());
    star = new Bound(star, bound_obj, 0.1);
  }
  star = scene->addObject(star);
  if (mass > 0 || name_mass != 0) {
    PointMass* pm = new PointMass(center, mass);
    if (name_mass != 0) {
      world_constants::values[name_mass] = &(pm->mass);
    }
    scene->addMass(pm);
  }
}

void AddBigStar(const Color& color1, const Color& color2, float perlin_scale,
                float ambient, float diffuse,
                float deformation_scale, float deformation_magnitude,
                float radius, const vec3& center, float mass,
                Scene* scene, char const* mass_name = 0, char const* radius_name = 0) {
  AddBigStar({color1, color2}, perlin_scale,
             ambient, diffuse,
             deformation_scale, deformation_magnitude,
             radius, center, mass, scene, mass_name, radius_name);
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

  SDF* bound_obj = new Sphere(vec3(), 1000, Material());
  bound_obj = new Negate(bound_obj);
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
    res->addLight(new PointLight(vec3(-25, 0, 0) + vec3::random() * 47));
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
  AddBigStar({Color(0xD14009), Color(0xFC9601), Color(0xFFCC33), Color(0xFFE484), Color(0xFFFFFF)}, 1, 5.0, 0.0, 2, 20, sun_radius, sun_center, 0, res);

  // // Earth.
  AddBigStar({Color(0xd8c596), Color(0x9fc164), Color(0xe9eff9), Color(0x6b93d6), Color(0x4f4cb0), Color(0x6b93d6), Color(0x4f4cb0), Color(0x6b93d6)}, 0.8, 0.1, 0.5, 0.8, 0.3, 8, vec3(20, -15, 5), 0, res);

  // // Jupiter.
  AddBigStar(Color(255, 0, 0), Color(100, 100, 100), 3, 0.1, 0.7, 0.1, 0.2, 5, vec3(-30, -30, 45), 0, res);

  // // Saturn.
  // AddBigStar(Color(161, 0, 0), Color(100, 50, 0), 15, 0.0, 0.01, 0.1, 0.1, 0.8, vec3(-15, -10, 50), 0, res);

  // // Uranus.
  // AddBigStar(Color(79, 76, 176), Color(106, 147, 214), 15, 0.0, 0.01, 0.1, 0.1, 0.5, vec3(20, 0, 30), 0, res);

  // Black hole.
  AddBigStar(Color(0, 0, 0), Color(0, 0, 0), 0, 0.0, 0.0, 0, 0, 7, vec3(-20, -15, 10), 5, res, "blackhole_mass", "blackhole_radius");

  SDF* bound_obj = new Sphere(vec3(), 1000, Material());
  bound_obj = new Negate(bound_obj);
  MultiUnion* stars_container = new MultiUnion();
  res->addObject(new Bound(stars_container, bound_obj, 10));
  
  const int NUM_BACKGROUND_STARS = 2000;
  // const int NUM_BACKGROUND_STARS = 500;
  for (int i = 0; i < NUM_BACKGROUND_STARS; ++i) {
    AddStarMultiUnion(stars_container, res);
  }

  for (int i = 0; i < 500; ++i) {
    res->addLight(new PointLight(sun_center + vec3::random() * (sun_radius + 2)));
  }

  res->addLight(new PointLight(vec3(100, 100, 2)));

  res->modifiable_rendering_params().camera_settings.eye_pos = vec3(0, 0, -200);
  res->modifiable_rendering_params().camera_settings.target = vec3(0, 0, 0);
  res->modifiable_rendering_params().do_shading = false;
  // res->modifiable_rendering_params().gravity_slowdown_factor = 500;
  res->modifiable_rendering_params().max_marching_steps = 50000;

  res->addLight(new DirectionalLight(vec3(-1, -1, -1)));

  res->modifiable_rendering_params().use_gravity = true;
  res->modifiable_rendering_params().animation_params.frames = 20;
}

void createStars3Scene(Scene *res) {
  // Black hole.
  vec3 blackhole_center(0, 0, 200);
  AddBigStar(Color(0, 0, 0), Color(0, 0, 0), 0, 0.0, 0.0, 0, 0, 1, blackhole_center, 5, res);

  const int num_orbiting_stars = 100;
  for (int i = 0; i < num_orbiting_stars; ++i) {
    float radius = rand_range(2, 100);
    float angle = rand_range(0, 2 * M_PI);
    float dx = radius * cos(angle);
    float dy = radius * sin(angle);
    vec3 pos = blackhole_center + vec3(dx, dy, 0);
    AddBigStar(Color(79, 76, 176), Color(216, 197, 150), 1.3, 0.1, 0.5, 0.1, 0.3, 0.5, pos, 0, res);
  }

  res->modifiable_rendering_params().camera_settings.eye_pos = vec3(0, 0, -200);
  res->modifiable_rendering_params().camera_settings.target = vec3(0, 0, 0);
  res->modifiable_rendering_params().do_shading = false;
  // res->modifiable_rendering_params().max_marching_steps = 50000;

  res->addLight(new DirectionalLight(vec3(-1, -1, 1)));

  // res->modifiable_rendering_params().use_gravity = true;
}

#endif
