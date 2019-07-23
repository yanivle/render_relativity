#ifndef INTERSECTABLE_OBJECT_H
#define INTERSECTABLE_OBJECT_H

#include "ray.h"

struct IntersectableObject {
  struct IntersectionResult {
    Color color;
    Vec3 position;
  };

  virtual void intersect(const Ray& ray, IntersectionResult& res) = 0;
};

#endif
