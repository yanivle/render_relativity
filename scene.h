#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <iostream>
#include "sdf.h"
#include "light.h"
#include "point_mass.h"
#include "rendering_params.h"

class Scene {
public:
  Scene() {
    std::cerr << "Creating new scene" << std::endl;
  }

  const RenderingParams& rendering_params() const {
    return rendering_params_;
  }

  RenderingParams* modifiable_rendering_params() {
    return &rendering_params_;
  }

  ~Scene() {
    std::cerr << "Deleting scene" << std::endl;
    for (int i = 0; i < objects_.size(); ++i) {
      delete objects_[i];
    }
    for (int i = 0; i < lights_.size(); ++i) {
      delete lights_[i];
    }
  }

  const SDF* root() const {
    return root_sdf;
  }

  SDF* own(SDF *sdf) {
    objects_.push_back(sdf);
    return sdf;
  }

  SDF* addObject(SDF *sdf) {
    if (root_sdf == 0) {
      root_sdf = sdf;
    } else {
      root_sdf = new Union(root_sdf, sdf);
    }

    return own(sdf);
  }

  void addMass(const PointMass& mass) {
    masses_.push_back(mass);
  }

  void addLight(Light* light) {
    lights_.push_back(light);
  }

  const std::vector<SDF*>& objects() const {
    return objects_;
  }

  const std::vector<Light*>& lights() const {
    return lights_;
  }

  const std::vector<PointMass>& masses() const {
    return masses_;
  }

private:
  RenderingParams rendering_params_;
  SDF* root_sdf;
  std::vector<SDF*> objects_;
  std::vector<Light*> lights_;
  std::vector<PointMass> masses_;
};

#endif
