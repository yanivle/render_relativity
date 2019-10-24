#ifndef RENDERER_H
#define RENDERER_H

#include "color.h"
#include "counters.h"
#include "mat4.h"
#include "scene.h"
#include "vec3.h"
#include "ray.h"

class Renderer {
 public:
  Renderer() {}
  Renderer(const Scene* scene) : scene_(scene) {}
  Renderer(const Mat4& view_world_matrix, const Scene* scene)
      : view_world_matrix_(view_world_matrix), scene_(scene) {}

  void setScene(Scene* scene) {
    scene_ = scene;
  }

  const mat4& view_world_matrix() const { return view_world_matrix_; }
  mat4& modifiable_view_world_matrix() { return view_world_matrix_; }

  struct IlluminationParams {
    vec3 intersection_point;
    vec3 ray_direction;
    vec3 normal;
    Material material;
    Color color_at_intersection;
    vec3 to_eye;
  };

  Color illuminate(const IlluminationParams& p, int remaining_depth) const {
      Color color;
      if (p.material.ambient > 0) {
        color += p.color_at_intersection * p.material.ambient;
      }
      if (p.material.diffuse > 0) {
        color += diffuse(p);
      }
      if (p.material.reflect > 0) {
        color += reflection(p, remaining_depth);
      }
      return color;
  }

  Color shoot(Ray ray, int remaining_depth) const {
    SDFResult r;
    int num_steps;
    bool hit = march(ray, &r, &num_steps);
    if (scene_->rendering_params().render_march_iterations) {
      return Palette::Veridis().color(double(num_steps) / 100);
    }
    if (hit) {
      IlluminationParams p;
      p.intersection_point = ray.origin;
      p.material = r.material;
      p.color_at_intersection = p.material.color(p.intersection_point);
      p.normal = scene_->root()->normal(p.intersection_point);
      p.ray_direction = ray.direction;
      // vec3 eye = view_world_matrix_ * scene_->rendering_params().camera_settings.eye_pos;
      vec3 eye = scene_->rendering_params().camera_settings.eye_pos;
      p.to_eye = (eye - p.intersection_point).normalize();
      return illuminate(p, remaining_depth);
    } else {
      return colors::BLACK;
    }
  }

 private:
  bool march(Ray& ray, SDFResult* res, int* num_steps) const {
    DEFINE_COUNTER(num_marching_steps);
    for (*num_steps = 0;
         *num_steps < scene_->rendering_params().max_marching_steps;
         ++(*num_steps)) {
      COUNTER_INC(num_marching_steps);
      *res = scene_->root()->sdf(ray.origin);
      if (res->dist < scene_->rendering_params().epsilon) {
        return true;
      } else if (abs(res->dist) > scene_->rendering_params().max_dist) {
        return false;
      }
      if (!scene_->rendering_params().use_gravity) {
        ray.march(res->dist);
      } else {
        ray.marchWithGravity(res->dist, scene_->masses());
      }
    }
    return false;
  }

  float shadow(Ray ray_to_light, float dist_to_light) const {
    float res = 1.0;
    const float k = 16;
    for (float t = scene_->rendering_params().epsilon * 100;
         t < dist_to_light;) {
      SDFResult r =
          scene_->root()->sdf(ray_to_light.origin + ray_to_light.direction * t);
      if (r.dist < scene_->rendering_params().epsilon) return 0.0;
      res = fmin(res, k * r.dist / t);
      t += r.dist;
    }
    return res;
  }

  Color diffuse(const IlluminationParams& p) const {
    Color color;
    for (int i = 0; i < scene_->lights().size(); ++i) {
      vec3 norm_to_light;
      float dist_to_light;
      float cos_alpha;
      scene_->lights()[i]->diffuse(p.intersection_point, p.normal, &norm_to_light, &dist_to_light,
                                   &cos_alpha);
      float shade = 1.0;
      if (scene_->rendering_params().do_shading) {
        shade = shadow(Ray(p.intersection_point, norm_to_light), dist_to_light);
      }
      float factor = std::max(0.0f, cos_alpha * shade);
      // if (scene_->rendering_params().light_decay) {
      //   factor *= 50 / (dist_to_light * dist_to_light);
      // }
      color += p.color_at_intersection * factor * p.material.diffuse;
      // TODO: can add light color here
      vec3 h = (p.to_eye + norm_to_light).normalize();
      factor = std::powf(std::max(0.0f, p.normal.dot(h)), p.material.shininess);
      // TODO: and add light color here too
      color += colors::WHITE * p.material.specular * factor;
    }
    return color;
  }

  Color reflection(const IlluminationParams& p, int remaining_depth) const {
    if (remaining_depth == 0) return colors::BLACK;
    Ray reflected_ray(p.intersection_point,
                      p.ray_direction.reflect(p.normal));
    int num_iters = 1;
    if (p.material.roughness > 0) {
      num_iters = scene_->rendering_params().roughness_iterations;
    }
    Color res;
    vec3 original_dir = reflected_ray.direction;
    for (int i = 0; i < num_iters; ++i) {
      if (p.material.roughness > 0) {
        // TODO: should the random() be replaced by a random orthonormal vec?
        vec3 noise_vec = Vec3::random() * p.material.roughness;
        reflected_ray.direction = original_dir + noise_vec;
        reflected_ray.direction.inormalize();
      }
      reflected_ray.march(scene_->rendering_params().epsilon * 5);
      res += shoot(reflected_ray, remaining_depth - 1);
    }
    return res * p.material.reflect / num_iters;
  }

  Mat4 view_world_matrix_;
  const Scene* scene_ = 0;
};

#endif
