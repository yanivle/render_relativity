#include "scenes.h"

namespace scenes {

DEFINE_SCENE(Spheres);

Spheres::Spheres() {
  setName("Spheres");
  SDF* s1 = new Sphere(vec3(-4.0, 2.5, 20), 4, Material(colors::RED  , 0., 0.45, 0.5, 0/*0.3*/, 0.3, 50));
  // SDF* s2 = new Sphere(vec3( 0.0, 0.0, 20), 1.5, Material(colors::BLUE , 0., 0.9, 0.9, 0.5, 0.0, 0));
  SDF* s3 = new Sphere(vec3( 4.5, 2.0, 20), 3.5, Material(colors::GREEN, 0., 0.45, 0.5, 0/*0.3*/, 0.3, 50));

  modifiable_rendering_params().reflection_depth = 5;
  modifiable_rendering_params().roughness_iterations = 5;
  modifiable_rendering_params().camera_settings.eye_pos = vec3(-15, 5, -10);
  modifiable_rendering_params().camera_settings.target = vec3(0, 0, 20);
  // modifiable_rendering_params().aa_factor = 2;

  addObject(s1);
  // addObject(s2);
  addObject(s3);

  // SDF* s4 = new Sphere(vec3(0, 0, 0), 100, Material(Color(0.5,0.5,1.0), 1.0, 0, 0, 0));
  // addObject(new Negate(s4));

  // addLight(new DirectionalLight(vec3(1, -1, 0)));
  addLight(new PointLight(vec3(-10, 10, 20)));
  addLight(new PointLight(vec3(0, 10, 15)));
  addLight(new PointLight(vec3(0, 10, 45)));

  vec3 plane_normal = vec3(0, 1, 0).normalize();
  vec3 cb1 = plane_normal.randomOrthonormalVec();
  vec3 cb2 = plane_normal.cross(cb1);
  Material floor_mat(Color(0.1,0.1,0.1)*5, 0., 0.5, 0.5, 0/*0.3*/);
  SDF* floor_plane = new Plane(plane_normal, cb1, cb2, floor_mat);
  addObject(new Translate(floor_plane, vec3(0, -1.5, 0)));

  // plane_normal = vec3(0, -1, 0).normalize();
  // cb1 = plane_normal.randomOrthonormalVec();
  // cb2 = plane_normal.cross(cb1);
  // Material ceil_mat(Color(0.5, 0.5, 1.0), 0.1, 1.0, 0.0);
  // SDF* ceil_plane = new Plane(plane_normal, cb1, cb2, ceil_mat);
  // addObject(new Translate(ceil_plane, vec3(0, 100, 0)));
}

}
