#include "scenes.h"

#include "../kdtree.h"
#include "../rgb.h"
#include "../rand_utils.h"

namespace scenes {

DEFINE_SCENE(Stars);

Sphere* createStar() {
  Color color = colors::WHITE;
  switch (rand() % 50) {
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
    color = colors::ORANGE;
    break;
  case 5:
    color = colors::CORAL;
    break;
  }
  float brightness = 5;
  while (rand() % 2 == 0 && brightness < 128) {
    brightness *= 2;
  }
  Material material(color, brightness, 0, 0, 0);
  vec3 center = vec3::random() * 1000;
  float radius = rand_range(0.3, 0.8);
  return new Sphere(center, radius, material);
}

void AddStarKDTree(SpheresKDTree* container) {
  container->addChild(createStar());
}

void AddBigStar(std::initializer_list<Color> colors, float perlin_scale,
                float ambient, float diffuse,
                float deformation_scale, float deformation_magnitude,
                float radius, const vec3& center, float mass,
                Scene* scene) {
  PerlinNoiseColorizer *colorizer = new PerlinNoiseColorizer(colors, perlin_scale);
  Material material(colorizer, ambient, diffuse, 0, 0);
  Sphere* sphere = new Sphere(center, radius, material);
  SDF* star = sphere;
  colorizer->setSurface((Sphere*)star);
  if (deformation_magnitude > 0) {
    star = new PerlinDeformation((Sphere*)star, deformation_scale, deformation_magnitude);
    SDF* bound_obj = new Sphere(center, radius + deformation_magnitude, Material());
    star = new Bound(star, bound_obj, 0.1);
  }
  star = scene->addObject(star);
  if (mass > 0) {
    PointMass* pm = new PointMass(center, mass);
    scene->addMass(pm);
  }
}

void AddBigStar(const Color& color1, const Color& color2, float perlin_scale,
                float ambient, float diffuse,
                float deformation_scale, float deformation_magnitude,
                float radius, const vec3& center, float mass,
                Scene* scene) {
  AddBigStar({color1, color2}, perlin_scale,
             ambient, diffuse,
             deformation_scale, deformation_magnitude,
             radius, center, mass, scene);
}

Stars::Stars() {
  setName("Stars");
  // Sun.
  vec3 sun_center(0, 0, 200);
  float sun_radius = 45;
  AddBigStar({Color(0xD14009), Color(0xFC9601), Color(0xFFCC33), Color(0xFFE484), Color(0xFFFFFF)}, 1, 5.0, 0.0, 2, 20, sun_radius, sun_center, 0, this);

  // Earth.
  AddBigStar({Color(0xd8c596), Color(0x9fc164), Color(0xe9eff9), Color(0x6b93d6), Color(0x4f4cb0), Color(0x6b93d6), Color(0x4f4cb0), Color(0x6b93d6)}, 0.8, 0.1, 0.5, 0.8, 0.3, 8, vec3(20, -15, 5), 0, this);

  // Jupiter.
  AddBigStar(Color(1, 0, 0), Color(0.39, 0.39, 0.39), 3, 0.1, 0.7, 0.1, 0.2, 5, vec3(-30, -30, 45), 0, this);

  // Black hole.
  AddBigStar(Color(0, 0, 0), Color(0, 0, 0), 0, 0.0, 0.0, 0, 0, 7, vec3(-20, -15, 10), 3, this);

  SDF* inner_sphere = new Sphere(vec3(), 999, Material());
  SDF* outer_sphere = new Sphere(vec3(), 1001, Material());
  SDF* bound_obj = new Intersection(new Negate(inner_sphere), outer_sphere);

  SpheresKDTree* kdtree = new SpheresKDTree();
  addObject(new Bound(kdtree, bound_obj, 1));
  
  const int NUM_BACKGROUND_STARS = 100000;
  // const int NUM_BACKGROUND_STARS = 1000;
  for (int i = 0; i < NUM_BACKGROUND_STARS; ++i) {
    AddStarKDTree(kdtree);
  }

  kdtree->compile();

  for (int i = 0; i < 500; ++i) {
    addLight(new PointLight(sun_center + vec3::random() * (sun_radius + 2)));
  }

  addLight(new PointLight(vec3(100, 100, 2)));

  modifiable_rendering_params().camera_settings.eye_pos = vec3(0, 0, -200);
  modifiable_rendering_params().camera_settings.target = vec3(0, 0, 0);
  modifiable_rendering_params().do_shading = false;
  // modifiable_rendering_params().gravity_slowdown_factor = 500;
  modifiable_rendering_params().max_marching_steps = 50000;

  addLight(new DirectionalLight(vec3(-1, -1, -1)));

  // modifiable_rendering_params().use_gravity = true;
  // modifiable_rendering_params().animation_params.frames = 100;
}

}

