#ifndef RENDERING_PARAMS
#define RENDERING_PARAMS

struct RenderingParams {
  int width = 1000;
  int height = 1000;
  int max_marching_steps = 500; // 500000
  float max_dist = 10000;
  float epsilon = 0.001;
  bool do_shading = true;
  int aa_factor = 1; // 4
  int reflection_depth = 5; // 1
  int roughness_iterations = 1; // 5
  bool use_gravity = false;
  bool light_decay = false;
  float screen_z = 0;
};

#endif
