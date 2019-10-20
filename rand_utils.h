#ifndef RAND_UTILS_H
#define RAND_UTILS_H

inline float rand_range(float min, float max) {
  return min + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(max - min)));
}

#endif
