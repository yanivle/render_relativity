#ifndef RENDERER_H
#define RENDERER_H

#include "mat4.h"
#include "vec3.h"
#include "color.h"
#include "scene.h"

class Renderer {
public:
  Renderer() {}
  Renderer(const Mat4& view_world_matrix) : view_world_matrix_(view_world_matrix) {}

  const mat4& view_world_matrix() const { return view_world_matrix_; }
  mat4& modifiable_view_world_matrix() { return view_world_matrix_; }

  Color shoot(Ray ray, const Scene& scene, int remaining_depth) {
    SDFResult r;
    ray.origin = view_world_matrix_ * ray.origin;
    ray.direction = view_world_matrix_.rotate(ray.direction);
    if(march(ray, scene, &r)) {
      Color ambient_color;
      if (r.material.ambient > 0) {
        ambient_color = r.material.color(ray.origin) * r.material.ambient;
      }
      Color diffuse_color;
      if (r.material.diffuse > 0) {
        diffuse_color = diffuse(ray.origin, r.material, scene, r.material.diffuse);
      }
      Color reflection_color;
      if (r.material.reflect > 0) {
        reflection_color = reflection(ray, scene, r.material, remaining_depth - 1);
      }
      Color color = ambient_color + diffuse_color + reflection_color * r.material.reflect;
      return color;
    } else {
      return colors::BLACK;
    }
  }

private:
  bool march(Ray& ray, const Scene& scene, SDFResult* res) {
    for (int i = 0; i < scene.rendering_params().max_marching_steps; ++i) {
      *res = scene.root()->sdf(ray.origin);
      if (res->dist < scene.rendering_params().epsilon) {
        return true;
      } else if (abs(res->dist) > scene.rendering_params().max_dist) {
        return false;
      }
      if (!scene.rendering_params().use_gravity) {
        ray.march(res->dist);
      } else {
        ray.marchWithGravity(res->dist, scene.masses(), scene.rendering_params().gravity_slowdown_factor);
      }
    }
    return false;
  }

  float shadow(Ray ray_to_light, const Scene& scene, float dist_to_light) {
    float res = 1.0;
    const float k = 16;
    for (float t = scene.rendering_params().epsilon * 100; t < dist_to_light;) {
      SDFResult r = scene.root()->sdf(ray_to_light.origin + ray_to_light.direction * t);
      if (r.dist < scene.rendering_params().epsilon) return 0.0;
      res = fmin(res, k * r.dist / t);
      t += r.dist;
    }
    return res;
  }

  Color diffuse(const vec3& v, const Material& material, const Scene& scene, float diffuse_factor) {
    vec3 normal = scene.root()->normal(v);
    Color color;
    float total_factor = 0;
    for (int i = 0; i < scene.lights().size(); ++i) {
      vec3 norm_to_light;
      float dist_to_light;
      float cos_alpha;
      scene.lights()[i]->diffuse(v, normal, &norm_to_light, &dist_to_light, &cos_alpha);
      float shade = 1.0;
      if (scene.rendering_params().do_shading) {
        shade = shadow(Ray(v, norm_to_light), scene, dist_to_light);
      }
      float factor = cos_alpha * shade;
      if (scene.rendering_params().light_decay) {
        factor = factor * 50 / (dist_to_light * dist_to_light);
      }
      if (factor < 0) factor = 0;
      total_factor += factor * diffuse_factor;
    }
    return material.color(v) * total_factor;
  }

  Color reflection(Ray ray, const Scene& scene, const Material& mat, int remaining_depth) {
    if (remaining_depth == 0) return colors::BLACK;
    vec3 normal = scene.root()->normal(ray.origin);
    ray.direction.ireflect(normal);
    int num_iters = 1;
    if (mat.roughness > 0) {
      num_iters = scene.rendering_params().roughness_iterations;
    }
    std::vector<Color> colors;
    vec3 original_dir = ray.direction;
    for (int i = 0; i < num_iters; ++i) {
      if (mat.roughness > 0) {
        vec3 noise_vec = Vec3::random() * mat.roughness;
        ray.direction = original_dir + noise_vec;
        ray.direction.inormalize();
      }
      ray.march(scene.rendering_params().epsilon * 5);
      colors.push_back(shoot(ray, scene, remaining_depth));
    }
    return Color::average(colors);
  }

  Mat4 view_world_matrix_;
};

#endif
