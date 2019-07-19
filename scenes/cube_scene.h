#ifndef CUBE_SCENE_H
#define CUBE_SCENE_H

#include "../scene.h"

SDF* createCube(Scene* scene, const FRGB& color = colors::RED) {
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
    SDF* side = scene->own(new Plane(normals[i], dummy, dummy, material));
    side = scene->own(new Translate(side, normals[i]));
    sides.push_back(side);
  }
  SDF* cube = sides[0];
  for (int i = 1; i < 6; ++i) {
    cube = scene->own(new Intersection(cube, sides[i]));
  }

  return cube;
}

void addCube(Scene* scene) {
  SDF* cube = createCube(scene);

  // chop off numbers
  SDF* sphere1 = scene->own(new Sphere(vec3(0, 0, 0), 1.5, Material(colors::WHITE, 0, 0.5, 0.05)));
  sphere1 = scene->own(new Negate(sphere1));
  cube = scene->own(new Intersection(cube, sphere1));

  // Rounded corners.
  // cube = scene->own(new Expand(cube, 0.1));

  cube = scene->own(new Scale(cube, 2));

  // Move to a better position.
  cube = scene->addObject(new Translate(cube, vec3(-1.5, 1.5, 15)));
}

void addChoppedSphere(Scene* scene) {
  SDF* cube = createCube(scene, colors::RED);
  cube = scene->own(new Scale(cube, 1.2));

  SDF* sphere = scene->own(new Sphere(vec3(0, 0, 0), 1.5, Material(colors::RED, 0, 0.5, 0.05)));
  sphere = scene->own(new Intersection(sphere, cube));


  // Move to a better position.
  sphere = scene->addObject(new Translate(sphere, vec3(1.5, -1.2, 8)));
}

void addCheckeredFloor(Scene* scene) {
  vec3 normal = vec3(0, 1, 0);
  vec3 cb1 = normal.randomOrthonormalVec();
  vec3 cb2 = normal.cross(cb1);
  CheckerboardColorizer *colorizer = new CheckerboardColorizer(FRGB(255, 165, 0), colors::BLACK, 0.3);
  Material material(colorizer, 0.1, 1.0, 0.5);
  SDF* floor = scene->own(new Plane(normal, cb1, cb2, material));
  colorizer->setSurface((Plane*)floor);
  floor = scene->addObject(new Translate(floor, vec3(0, -2, 20)));
}

void addCheckeredWall(Scene* scene) {
  vec3 normal = vec3(0, 0, -1);
  vec3 cb1 = normal.randomOrthonormalVec();
  vec3 cb2 = normal.cross(cb1);
  CheckerboardColorizer *colorizer = new CheckerboardColorizer(colors::BLUE, colors::BLACK, 1);
  Material material(colorizer, 0, 0.5, 0.5);
  SDF* wall = scene->own(new Plane(normal, cb1, cb2, material));
  colorizer->setSurface((Plane*)wall);
  wall = scene->addObject(new Translate(wall, vec3(0, 0, 20)));
}

void createCubeScene(Scene *scene) {
  scene->modifiable_rendering_params().reflection_depth = 1;
  addCube(scene);
  addChoppedSphere(scene);
  addCheckeredFloor(scene);
  addCheckeredWall(scene);

  // scene->addLight(new PointLight(vec3(10, 20, -10)));
  scene->addLight(new SpotLight(vec3(-1.5, 1.5, 0), vec3(0, 0, 1), M_PI/10));
  scene->addLight(new SpotLight(vec3(1.5, -1.2, 0), vec3(0, 0, 1), M_PI/10));
  // scene->addLight(new SpotLight(vec3(10, 20, -10), vec3(-10, -20, 10), M_PI/10));

  // scene->addLight(new PointLight(vec3(0, 0, -10)));
  scene->addLight(new PointLight(vec3(10, 20, -10)));
}

#endif
