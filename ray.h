#ifndef RAY_H
#define RAY_H

#include "vec3.h"
#include <string>
#include <strstream>
#include "point_mass.h"

struct Ray {
    vec3 origin;
    vec3 direction;

    Ray(const vec3& origin, const vec3& direction):
    origin(origin), direction(direction) {
      this->direction.inormalize();
    }

    void march(float distance) {
      origin += direction * distance;
    }

    void marchWithGravity(float distance, const std::vector<PointMass>& masses) {
      const float GRAVITY_SLOWDOWN_FACTOR = 1;
      vec3 total_force;
      for (int i = 0; i < masses.size(); ++i) {
        vec3 to_mass = masses[i].v - origin;
        float dist_to_mass_2 = to_mass.len2();
        vec3 norm_to_mass = to_mass.normalize();
        total_force += norm_to_mass * (masses[i].mass / dist_to_mass_2);
      }
      direction += total_force / GRAVITY_SLOWDOWN_FACTOR;
      direction.inormalize();
      origin += direction * distance / GRAVITY_SLOWDOWN_FACTOR;
    }

    std::string str() const {
      std::strstream res;
      res << "Ray(origin=" << origin.str() << ", direction=" << direction.str() << ')';
      return res.str();
    }
};

#endif
