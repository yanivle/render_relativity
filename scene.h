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
    root_sdf = new MultiUnion();
  }

  const RenderingParams& rendering_params() const {
    return rendering_params_;
  }

  RenderingParams& modifiable_rendering_params() {
    return rendering_params_;
  }

  ~Scene() {
    std::cerr << "Deleting scene" << std::endl;
    for (int i = 0; i < objects_.size(); ++i) {
      delete objects_[i];
    }
    for (int i = 0; i < lights_.size(); ++i) {
      delete lights_[i];
    }
    for (int i = 0; i < masses_.size(); ++i) {
      delete masses_[i];
    }
  }

  const std::string name() const {
    return name_;
  }

  void setName(const std::string& name) {
    name_ = name;
  }

  const SDF* root() const {
    return root_sdf;
  }

  SDF* addObject(SDF *sdf) {
    root_sdf->addChild(sdf);

    return sdf;
  }

  void addMass(PointMass* mass) {
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

  const std::vector<PointMass*>& masses() const {
    return masses_;
  }

  std::string str() const {
    std::ostringstream ss;
    ss << "Scene: " << name() << std::endl;
    ss << "Resolution: " << rendering_params().width << 'x'
                         << rendering_params().height << std::endl;
    ss << "Total masses: " << masses().size() << std::endl;
    ss << "Total lights: " << lights().size() << std::endl;
    return ss.str();
  }

private:
  RenderingParams rendering_params_;
  MultiUnion* root_sdf = 0;
  std::vector<SDF*> objects_;
  std::vector<Light*> lights_;
  std::vector<PointMass*> masses_;
  std::string name_ = "unnamed";
};

#endif
