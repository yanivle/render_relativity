#ifndef SDF_H
#define SDF_H

#include "vec3.h"
#include <algorithm>
#include <vector>
#include "color.h"
#include "material.h"
#include "colorizer.h"
#include "perlin_noise.h"

struct SDFResult {
  SDFResult() {}

  SDFResult(float dist, const Material& material):
    dist(dist), material(material) {}

  float dist;
  Material material;
};

class SDF {
public:
  virtual SDFResult sdf(const vec3& v) const = 0;
  virtual ~SDF() {}

  vec3 normal(const vec3& v) const {
    const float e = 0.0001;
    const vec3 e_x = vec3(e, 0, 0);
    const vec3 e_y = vec3(0, e, 0);
    const vec3 e_z = vec3(0, 0, e);

    vec3 res;
    res.x = sdf(v + e_x).dist - sdf(v - e_x).dist;
    res.y = sdf(v + e_y).dist - sdf(v - e_y).dist;
    res.z = sdf(v + e_z).dist - sdf(v - e_z).dist;
    return res.normalize();
  }
};

class ParametrizableSurfaceSDF : public SDF, public ParametrizableSurface {
};

class Sphere : public ParametrizableSurfaceSDF {
public:
  Sphere(const vec3& center, float radius, const Material& material) :
    center(center), radius(radius), material(material) {}

  void coordinates(const vec3& vec, float* u, float* v) const {
    vec3 relative = (vec - center).normalize();
    *u = acos(relative.y) * 30;
    // *u = acos(relative.z);
    *v = atan(relative.x / relative.z) * 30;
  }

  SDFResult sdf(const vec3& v) const {
    float dist = (v - center).len() - radius;
    SDFResult res(dist, material);
    return res;
  }

private:
  vec3 center;
  float radius;
  Material material;
};

class PerlinDeformation : public SDF {
public:
  PerlinDeformation(ParametrizableSurfaceSDF* child, float scale, float magnitude)
    : child(child), scale(scale), magnitude(magnitude) {}

  SDFResult sdf(const vec3& v) const {
    float a, b;
    child->coordinates(v, &a, &b);
    float alpha = perlin.noise(a * scale, b * scale, 0);
    SDFResult res = child->sdf(v);
    res.dist += magnitude * alpha;
    return res;
  }
private:
  ParametrizableSurfaceSDF* child;
  float scale;
  float magnitude;
  PerlinNoise perlin;
};

class Plane : public ParametrizableSurfaceSDF {
public:
  Plane(const vec3& normal,
        const vec3& checkerboard_axis1,
        const vec3& checkerboard_axis2,
        const Material& material) :
    normal(normal),
    checkerboard_axis1(checkerboard_axis1),
    checkerboard_axis2(checkerboard_axis2),
    material(material) {}

  void coordinates(const vec3& vec, float* u, float* v) const {
    *u = checkerboard_axis1.dot(vec);
    *v = checkerboard_axis2.dot(vec);
  }

  SDFResult sdf(const vec3& v) const {
    SDFResult res(v.dot(normal), material);
    return res;
  }

private:
  vec3 normal;
  vec3 checkerboard_axis1, checkerboard_axis2;
  Material material;
};

class MultiUnion : public SDF {
public:
  void addChild(SDF* child) {
    children.push_back(child);
  }

  SDFResult sdf(const vec3& v) const {
    SDFResult res(1, Material());
    const float bound = 0;
    for (int i = 0; i < children.size(); ++i) {
      SDFResult r = children[i]->sdf(v);
      if (r.dist < res.dist) {
        res = r;
        if (res.dist < bound) {
          SDF* t = children[0];
          children[0] = children[i];
          children[i] = t;
          break;
        }
      }
    }
    return res;
  }

private:
  mutable std::vector<SDF *>children;
};

class Union : public SDF {
public:
  Union(SDF *obj1, SDF* obj2): obj1(obj1), obj2(obj2) {}

  SDFResult sdf(const vec3& v) const {
    SDFResult r1 = obj1->sdf(v);
    // TODO: can I make bound tighter?
    const float bound = 0;
    if (r1.dist < bound) {
      return r1;
    }
    SDFResult r2 = obj2->sdf(v);
    if (r1.dist < r2.dist) {
      return r1;
    }
    // Swap them, as maybe nearby rays will trigger r2 first.
    SDF* t = obj1; obj1 = obj2; obj2 = t;
    return r2;
  }

private:
  mutable SDF *obj1;
  mutable SDF *obj2;
};

class Intersection : public SDF {
public:
  Intersection(SDF *obj1, SDF* obj2): obj1(obj1), obj2(obj2) {}

  SDFResult sdf(const vec3& v) const {
    SDFResult r1 = obj1->sdf(v);
    // TODO: can I make bound tighter?
    const float bound = 1;
    if (r1.dist > bound) {
      return r1;
    }
    SDFResult r2 = obj2->sdf(v);
    if (r1.dist > r2.dist) {
      return r1;
    }
    // Swap them, as maybe nearby rays will trigger r2 first.
    SDF* t = obj1; obj1 = obj2; obj2 = t;
    return r2;
  }

private:
  mutable SDF *obj1;
  mutable SDF *obj2;
};

class Smooth : public SDF {
public:
  Smooth(SDF *obj1, SDF* obj2, float k): obj1(obj1), obj2(obj2), k(k) {}

  SDFResult sdf(const vec3& v) const {
    SDFResult r1 = obj1->sdf(v);
    SDFResult r2 = obj2->sdf(v);
    float e1 = exp2(-k * r1.dist);
    float e2 = exp2(-k * r2.dist);
    float dist = -log2(e1 + e2) / k; // (r1.dist * e1 + r2.dist * e2) / (e1 + e2);
    float alpha = e2 / (e1 + e2);
    Material mat = r1.material;
    mat.color_ = interpolate_colors(alpha, r1.material.color_, r2.material.color_);
    mat.ambient = interpolate_floats(1 - alpha, r1.material.ambient, r2.material.ambient);
    mat.diffuse = interpolate_floats(1 - alpha, r1.material.diffuse, r2.material.diffuse);
    mat.reflect = interpolate_floats(1 - alpha, r1.material.reflect, r2.material.reflect);
    return SDFResult(dist, mat);
  }

private:
  SDF *obj1;
  SDF *obj2;
  float k;
};

class Periodic : public SDF {
public:
  Periodic(SDF *child, const vec3& period): child(child), period(period) {}

  SDFResult sdf(const vec3& v) const {
    SDFResult res = child->sdf(v.mod(period) - period * 0.5);
    return res;
  }

private:
  SDF *child;
  vec3 period;
};

class Translate : public SDF {
public:
  Translate(SDF *child, const vec3& t): child(child), t(t) {}

  SDFResult sdf(const vec3& v) const {
    return child->sdf(v - t);
  }

private:
  SDF *child;
  vec3 t;
};

class Expand : public SDF {
public:
  Expand(SDF *child, float r): child(child), r(r) {}

  SDFResult sdf(const vec3& v) const {
    SDFResult res = child->sdf(v);
    res.dist -= r;
    return res;
  }

private:
  SDF *child;
  float r;
};

class Scale : public SDF {
public:
  Scale(SDF *child, float r): child(child), r(r) {}

  SDFResult sdf(const vec3& v) const {
    SDFResult res = child->sdf(v / r);
    res.dist *= r;
    return res;
  }

private:
  SDF *child;
  float r;
};

class PointwiseMultiply : public SDF {
public:
  PointwiseMultiply(SDF *child, const vec3& t): child(child), t(t) {}

  SDFResult sdf(const vec3& v) const {
    return child->sdf(vec3(v.x * t.x, v.y * t.y, v.z * t.z));
  }

private:
  SDF *child;
  vec3 t;
};


class Negate : public SDF {
public:
  Negate(SDF *child): child(child) {}

  SDFResult sdf(const vec3& v) const {
    SDFResult res = child->sdf(v);
    res.dist = -res.dist;
    return res;
  }

private:
  SDF *child;
};

class Bound : public SDF {
 public:
  Bound(SDF *child, SDF* bound_sdf, float bound_dist)
    : child(child), bound_sdf(bound_sdf), bound_dist(bound_dist) {}

  SDFResult sdf(const vec3& v) const {
    SDFResult bound_res = bound_sdf->sdf(v);
    if (bound_res.dist > bound_dist) {
      return bound_res;
    }
    return child->sdf(v);
  }

private:
  SDF *child;
  SDF *bound_sdf;
  float bound_dist = 1;
};

#endif
