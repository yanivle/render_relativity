#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "sdf.h"

class Scene {
public:
  ~Scene() {
    for (int i = 0; i < sdfs.size(); ++i) {
      delete sdfs[i];
    }
  }

  SDF* root() {
    return root;
  }

  void Add(SDF *sdf) {
    if (root == 0) {
      root = sdf;
    } else {
      root = new Union(root, sdf);
    }

    sdfs.push_back(sdf);
  }

private:
  SDF* root;
  std::vector<SDF*> sdfs;
};

#endif
