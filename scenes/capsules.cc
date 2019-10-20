#include "scenes.h"

namespace scenes {

DEFINE_SCENE(Capsules);

Capsules::Capsules() {
  setName("Capsules");
  SDF* s1 = new Sphere(vec3(0, 0, 0), 1.5,
                       Material(colors::RED, 0.1, 1.0, 0.5, 0, 1.0, 300));
  SDF* s2 = new Sphere(vec3(0, 0, 0), 1.5,
                       Material(colors::YELLOW, 0.1, 1.0, 0.5, 0, 1.0, 300));
  SDF* s3 = new Sphere(vec3(0, 0, 0), 1.5,
                       Material(colors::GREEN, 0.1, 1.0, 0.5, 0, 1.0, 300));
  SDF* s4 = new Sphere(vec3(0, 0, 0), 1.5,
                       Material(colors::BLUE, 0.1, 1.0, 0.5, 0, 1.0, 300));

  addMass(new PointMass(vec3(-1.7, 2, 16), 0.001));
  addMass(new PointMass(vec3(-1.7, 0, 16), 0.001));
  addMass(new PointMass(vec3(1.7, 2, 16), 0.001));
  addMass(new PointMass(vec3(1.7, 0, 16), 0.001));

  s1 = new Translate(s1, vec3(-1.7, 2, 16));
  s2 = new Translate(s2, vec3(-1.7, 0, 16));
  s3 = new Translate(s3, vec3(1.7, 2, 16));
  s4 = new Translate(s4, vec3(1.7, 0, 16));

  addObject(new Smooth(s1, s2, 5));
  addObject(new Smooth(s3, s4, 5));

  vec3 plane_normal = vec3(0, 1, -0).normalize();
  vec3 cb1 = plane_normal.randomOrthonormalVec();
  vec3 cb2 = plane_normal.cross(cb1);
  // std::cout << plane_normal.str() << " " << cb1.str() << " " << cb2.str() <<
  // std::endl;
  CheckerboardColorizer* floor_colorizer =
      new CheckerboardColorizer(colors::WHITE, colors::BLACK, 0.3);
  Material floor_mat(floor_colorizer, 0.1, 1.0, 0.9);
  SDF* floor_plane = new Plane(plane_normal, cb1, cb2, floor_mat);
  floor_colorizer->setSurface((Plane*)floor_plane);
  floor_plane = addObject(new Translate(floor_plane, vec3(0, -2, 20)));

  plane_normal = vec3(0, -0, -1).normalize();
  cb1 = plane_normal.randomOrthonormalVec();
  cb2 = plane_normal.cross(cb1);
  // std::cout << plane_normal.str() << " " << cb1.str() << " " << cb2.str() <<
  // std::endl;
  CheckerboardColorizer* wall_colorizer =
      new CheckerboardColorizer(colors::WHITE, colors::BLACK, 0.3);
  Material wall_mat(wall_colorizer, 0.1, 1.0, 0.9);
  SDF* wall = new Plane(plane_normal, cb1, cb2, wall_mat);
  wall_colorizer->setSurface((Plane*)wall);
  wall = addObject(new Translate(wall, vec3(0, 0, 20)));

  addLight(new SpotLight(vec3(3, 10, -10), vec3(-0.25, -0.7, 2), M_PI / 14));
}

}  // namespace scenes
