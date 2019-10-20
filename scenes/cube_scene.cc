#include "scenes.h"

namespace scenes {

DEFINE_SCENE(ChoppedCube);


SDF* createCube(Scene* scene, const Color& color = colors::RED) {
  Material material(color, 0, 0.5, 0.05, 0.0, 1.0, 10000);
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

void addCube(Scene* scene) {
  SDF* cube = createCube(scene);

  // chop off numbers
  SDF* sphere1 = new Sphere(vec3(0, 0, 0), 1.5, Material(colors::WHITE, 0, 0.5, 0.05, 0.0, 1.0, 300));
  sphere1 = new Negate(sphere1);
  cube = new Intersection(cube, sphere1);

  // Rounded corners.
  // cube = scene->own(new Expand(cube, 0.1));

  cube = new Scale(cube, 2);

  // Move to a better position.
  cube = scene->addObject(new Translate(cube, vec3(-1.5, 1.5, 15)));
}

void addChoppedSphere(Scene* scene) {
  SDF* cube = createCube(scene, colors::RED);
  cube = new Scale(cube, 1.2);

  SDF* sphere = new Sphere(vec3(0, 0, 0), 1.5, Material(colors::RED, 0, 0.5, 0.05, 0.0, 1.0, 300));
  sphere = new Intersection(sphere, cube);


  // Move to a better position.
  sphere = scene->addObject(new Translate(sphere, vec3(1.5, -1.2, 8)));
  scene->addMass(new PointMass(vec3(1.5, -1.2, 8), 0.01));
}

void addCheckeredFloor(Scene* scene) {
  vec3 normal = vec3(0, 1, 0);
  vec3 cb1 = normal.randomOrthonormalVec();
  vec3 cb2 = normal.cross(cb1);
  CheckerboardColorizer *colorizer = new CheckerboardColorizer(Color(1.0, 0.65, 0), colors::BLACK, 0.3);
  Material material(colorizer, 0., 0.9, 0.5, 0.0, 1.0, 10000);
  SDF* floor = new Plane(normal, cb1, cb2, material);
  colorizer->setSurface((Plane*)floor);
  floor = scene->addObject(new Translate(floor, vec3(0, -2, 20)));
}

void addCheckeredWall(Scene* scene) {
  vec3 normal = vec3(0, 0, -1);
  vec3 cb1 = normal.randomOrthonormalVec();
  vec3 cb2 = normal.cross(cb1);
  CheckerboardColorizer *colorizer = new CheckerboardColorizer(colors::BLUE, colors::BLACK, 1);
  Material material(colorizer, 0, 0.5, 0.5, 0.0, 0.5, 10000);
  SDF* wall = new Plane(normal, cb1, cb2, material);
  colorizer->setSurface((Plane*)wall);
  wall = scene->addObject(new Translate(wall, vec3(0, 0, 20)));
}

ChoppedCube::ChoppedCube() {
  setName("Cube");
  modifiable_rendering_params().reflection_depth = 1;
  addCube(this);
  addChoppedSphere(this);
  addCheckeredFloor(this);
  addCheckeredWall(this);

  // // addLight(new PointLight(vec3(10, 20, -10)));
  addLight(new SpotLight(vec3(-1.5, 1.5, 0), vec3(0, 0, 1), M_PI/10));
  addLight(new SpotLight(vec3(1.5, -1.2, 0), vec3(0, 0, 1), M_PI/10));
  // // addLight(new SpotLight(vec3(10, 20, -10), vec3(-10, -20, 10), M_PI/10));

  // addLight(new PointLight(vec3(1, 2, -10)));
  addLight(new PointLight(vec3(10, 20, -10)));

  modifiable_rendering_params().camera_settings.eye_pos = vec3(0, 0, -5);
  modifiable_rendering_params().camera_settings.target = vec3(0, 0, 0);
  modifiable_rendering_params().use_gravity = false;
}
  
}
