#include <iostream>
#include <thread>
#include <vector>

#include "../kdtree.h"

#include "catch.hpp"

TEST_CASE("KDTree sanity", "[KDTree]") {
  SpheresKDTree* kdtree = new SpheresKDTree();

  for (int x = -10; x < 10; ++x) {
    for (int y = -10; y < 10; ++y) {
      for (int z = -10; z < 10; ++z) {
        Sphere* s = new Sphere(vec3(x, y, z), 0.2, Material(Color(x, y, z)));
        kdtree->addChild(s);
      }
    }
  }

  SDFResult r = kdtree->sdf(vec3(3.1, 3.1, 3.1));
  CHECK(r.material.color_ == Color(3, 3, 3));

  kdtree->compile();

  r = kdtree->sdf(vec3(5.9, 5.9, 5.9));
  CHECK(r.material.color_ == Color(6, 6, 6));
}
