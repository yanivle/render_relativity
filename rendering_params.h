#ifndef RENDERING_PARAMS
#define RENDERING_PARAMS

#include "vec3.h"

struct RenderingParams {
  int width = 1024;
  int height = 1024;
  int max_marching_steps = 500;  // 500000
  float max_dist = 10000;
  float epsilon = 0.001;
  bool do_shading = true;
  int aa_factor = 1;             // 4
  int reflection_depth = 5;      // 1
  int roughness_iterations = 1;  // 5
  bool use_gravity = false;
  bool light_decay = false;
  float screen_z = 5;

  bool render_march_iterations = false;

  struct AnimationParams {
    int frames = 1;
    float time_delta = 0.001;
  } animation_params;
  struct CameraSettings {
    vec3 eye_pos = vec3(0, 0, 0);
    vec3 target = vec3(0, 0, 1);
    vec3 up = vec3(0, 1, 0);
  } camera_settings;
};

#endif
