#ifndef KDTREE
#define KDTREE

#include <vector>

#include "vec3.h"
#include "logging.h"
#include "sdf.h"

class SpheresKDTree : public SDF {
  struct Pivot {
    vec3::Axis axis;
    float value;
    std::vector<Sphere*> spheres;
  };

public:
  void addChild(Sphere* child) {
      pivot.spheres.push_back(child);
  }

  static float spread(const std::vector<float>& v) {
      const auto [min, max] = std::minmax_element(begin(v), end(v));
      return *max - *min;
  }

  struct SphereComparatorByAxis {
    SphereComparatorByAxis(vec3::Axis axis) : axis(axis) {}

    vec3::Axis axis;

    bool operator()(const Sphere *a, const Sphere *b) {
        return a->center[axis] < b->center[axis];
    }
  };

  void addLeftChild(Sphere* s) {
      if (left == 0) {
          left = new SpheresKDTree;
      }
      left->pivot.spheres.push_back(s);
  }

  void addRightChild(Sphere* s) {
      if (right == 0) {
          right = new SpheresKDTree;
      }
      right->pivot.spheres.push_back(s);
  }

  void compile() {
      if (pivot.spheres.size() < 3) {
          return;
      }
      std::vector<float> xs, ys, zs;
      for (Sphere* s : pivot.spheres) {
          xs.push_back(s->center.x);
          ys.push_back(s->center.y);
          zs.push_back(s->center.z);
      }
      float x_spread = spread(xs);
      float y_spread = spread(ys);
      float z_spread = spread(zs);
      Pivot new_pivot;
      if (x_spread > y_spread && x_spread > z_spread) {
          new_pivot.axis = vec3::X;
      } else if (y_spread > z_spread) {
          new_pivot.axis = vec3::Y;
      } else {
          new_pivot.axis = vec3::Z;
      }
      SphereComparatorByAxis cmp(new_pivot.axis);
      int median_index = pivot.spheres.size() / 2;
      std::nth_element(pivot.spheres.begin(),
                       pivot.spheres.begin() + median_index,
                       pivot.spheres.end(),
                       cmp);
      Sphere* median_sphere = pivot.spheres[median_index];
      new_pivot.value = median_sphere->center[new_pivot.axis];
      for (int i = 0; i < pivot.spheres.size(); ++i) {
          Sphere* sphere = pivot.spheres[i];
          float coord = sphere->center[new_pivot.axis];
          if (std::abs(new_pivot.value - coord) > sphere->radius) {
              if (i < median_index) {
                addLeftChild(sphere);
              } else {
                addRightChild(sphere);
              }
          } else {
              new_pivot.spheres.push_back(sphere);
          }
      }
      pivot = new_pivot;
      if (left != 0) {
          left->compile();
      }
      if (right != 0) {
          right->compile();
      }
  }

  SDFResult sdf(const vec3& v) const {
    SDF_COUNTERS(SpheresKDTree);
    // TODO: clean this constant.
    SDFResult res(1000000000, Material());
    for (Sphere* sphere : pivot.spheres) {
      SDFResult r = sphere->sdf(v);
      if (r.dist < res.dist) {
        res = r;
        if (res.dist < 0) {
          return res;
        }
      }
    }

    SpheresKDTree* subtree;
    if (v[pivot.axis] < pivot.value) {
        subtree = left;
    } else {
        subtree = right;
    }    
    if (subtree != 0) {
      SDFResult r = subtree->sdf(v);
      if (r.dist < res.dist) {
          return r;
      }
    }
    return res;
  }
private:
  SpheresKDTree* left = 0;
  SpheresKDTree* right = 0;
  Pivot pivot;
};

#endif
