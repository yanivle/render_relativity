#ifndef MAT4_H
#define MAT4_H

#include "vec3.h"

class Mat4 {
public:
  Mat4() {
    for (int i = 0; i < 16; ++i) {
      d[i] = 0;
    }
  }

  static Mat4 lookat(const vec3& eye, const vec3& target, const vec3& up) {
    vec3 forward = (target - eye).normalize();
    vec3 side = forward.cross(up).normalize();
    // TODO: Recomputing up as per original implementation - need to think
    vec3 up_ = side.cross(forward); // No need to normalize, as forward and side are.

    Mat4 res;
    res.d[0] = side.x;
    res.d[4] = side.y;
    res.d[8] = side.z;
    res.d[12] = 0.0;
    // --------------------
    res.d[1] = up_.x;
    res.d[5] = up_.y;
    res.d[9] = up_.z;
    res.d[13] = 0.0;
    // --------------------
    res.d[2] = forward.x;
    res.d[6] = forward.y;
    res.d[10] = forward.z;
    res.d[14] = 0.0;
    // --------------------
    res.d[3] = res.d[7] = res.d[11] = 0.0;
    res.d[15] = 1.0;
    return res;
  }

  float& operator()(int i) {
    return d[i];
  }

  const float& operator()(int i) const {
    return d[i];
  }

  float& operator()(int x, int y) {
    return d[y * 4 + x];
  }

  const float& operator()(int x, int y) const {
    return d[y * 4 + x];
  }

  void operator*= (float a) {
    for (int i = 0; i < 16; ++i) d[i] *= a;
  }

  void operator+= (const Mat4& other) {
    for (int i = 0; i < 16; ++i) d[i] += other.d[i];
  }

  void operator-= (const Mat4& other) {
    for (int i = 0; i < 16; ++i) d[i] -= other.d[i];
  }

  void operator-() {
    for (int i = 0; i < 16; ++i) d[i] = -d[i];
  }

  Mat4 operator* (const Mat4& other) {
    Mat4 res;
    for (int y = 0; y < 4; ++y) {
      for (int x = 0; x < 4; ++x) {
        float sum = 0;
        for (int i = 0; i < 4; ++i) {
          sum += (*this)(y, i) * other(i, x);
        }
        res(x, y) = sum;
      }
    }
    return res;
  }

  // This is assuming the special form of this matrix:
  // [* * * *]
  // [* * * *]
  // [* * * *]
  // [0 0 0 1]
  vec3 operator* (const vec3& v) {
    float x = d[0 + 0] * v.x + d[0 + 1] * v.y + d[0 + 2] * v.z + d[0 + 3];
    float y = d[4 + 0] * v.x + d[4 + 1] * v.y + d[4 + 2] * v.z + d[4 + 3];
    float z = d[8 + 0] * v.x + d[8 + 1] * v.y + d[8 + 2] * v.z + d[8 + 3];
    return vec3(x, y, z);
  }

protected:
  float d[16];
};

typedef Mat4 mat4;

#endif
