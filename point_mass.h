#ifndef POINTMASS_H
#define POINTMASS_H

struct PointMass {
  PointMass(const vec3& v, float mass) : v(v), mass(mass) {}
  vec3 v;
  float mass;
};

#endif
