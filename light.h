#ifndef LIGHT_H
#define LIGHT_H

class Light {
public:
  virtual ~Light() {}
  virtual void diffuse(const vec3& point, const vec3& normal, vec3 *norm_to_light, float *dist_to_light, float* res) const = 0;
};

class PointLight : public Light {
public:
  PointLight(const vec3& pos) : pos(pos) {}

  virtual void diffuse(const vec3& point, const vec3& normal, vec3 *norm_to_light, float *dist_to_light, float* res) const {
    vec3 vec_to_light = pos - point;
    *dist_to_light = vec_to_light.len();
    *norm_to_light = vec_to_light / *dist_to_light;
    *res = normal.dot(*norm_to_light);
  }

protected:
  vec3 pos;
};

class DirectionalLight : public Light {
public:
  DirectionalLight(const vec3& dir) : dir(dir) {
    this->dir.inormalize();
  }

  virtual void diffuse(const vec3& point, const vec3& normal, vec3 *norm_to_light, float *dist_to_light, float* res) const {
    *norm_to_light = -dir;
    *dist_to_light = 1;
    *res = normal.dot(*norm_to_light);
  }

private:
  vec3 dir;
};

class SpotLight : public PointLight {
public:
  SpotLight(const vec3& pos, const vec3& dir, float angle) : PointLight(pos), dir(dir) {
    this->dir.inormalize();
    cos_angle = cos(angle);
  }

  virtual void diffuse(const vec3& point, const vec3& normal, vec3 *norm_to_light, float *dist_to_light, float* res) const {
    PointLight::diffuse(point, normal, norm_to_light, dist_to_light, res);
    if (norm_to_light->dot(-dir) < cos_angle) {
      *res = 0;
    }
  }

private:
  vec3 dir;
  float cos_angle;
};

#endif
