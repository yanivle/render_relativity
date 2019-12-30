#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <ostream>
#include <sstream>
#include <string>

#include "logging.h"

struct Vec3 {
  float x, y, z;

  Vec3() : x(0), y(0), z(0) {}
  Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

  void operator+=(const Vec3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
  }

  void operator-=(const Vec3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
  }

  void operator*=(float alpha) {
    x *= alpha;
    y *= alpha;
    z *= alpha;
  }

  void operator/=(float alpha) {
    x /= alpha;
    y /= alpha;
    z /= alpha;
  }

  Vec3 operator+(const Vec3& other) const {
    Vec3 res = *this;
    res += other;
    return res;
  }

  Vec3 operator-(const Vec3& other) const {
    Vec3 res = *this;
    res -= other;
    return res;
  }

  Vec3 operator-() const { return Vec3() - *this; }

  Vec3 operator*(float alpha) const {
    Vec3 res = *this;
    res *= alpha;
    return res;
  }

  Vec3 operator/(float alpha) const {
    Vec3 res = *this;
    res /= alpha;
    return res;
  }

  // This allows things like v.xyx()
  #define SLICED_ACCESSOR(a, b, c) Vec3 a##b##c() const { return Vec3(a, b, c); }
  SLICED_ACCESSOR(x, x, x)
  SLICED_ACCESSOR(x, x, y)
  SLICED_ACCESSOR(x, x, z)
  SLICED_ACCESSOR(x, y, x)
  SLICED_ACCESSOR(x, y, y)
  SLICED_ACCESSOR(x, y, z)
  SLICED_ACCESSOR(x, z, x)
  SLICED_ACCESSOR(x, z, y)
  SLICED_ACCESSOR(x, z, z)
  SLICED_ACCESSOR(y, x, x)
  SLICED_ACCESSOR(y, x, y)
  SLICED_ACCESSOR(y, x, z)
  SLICED_ACCESSOR(y, y, x)
  SLICED_ACCESSOR(y, y, y)
  SLICED_ACCESSOR(y, y, z)
  SLICED_ACCESSOR(y, z, x)
  SLICED_ACCESSOR(y, z, y)
  SLICED_ACCESSOR(y, z, z)
  SLICED_ACCESSOR(z, x, x)
  SLICED_ACCESSOR(z, x, y)
  SLICED_ACCESSOR(z, x, z)
  SLICED_ACCESSOR(z, y, x)
  SLICED_ACCESSOR(z, y, y)
  SLICED_ACCESSOR(z, y, z)
  SLICED_ACCESSOR(z, z, x)
  SLICED_ACCESSOR(z, z, y)
  SLICED_ACCESSOR(z, z, z)

  enum Axis { X = 0, Y = 1, Z = 2 };

  float operator[](Axis axis) const {
    switch (axis) {
      case X:
        return x;
      case Y:
        return y;
      case Z:
        return z;
      default:
        CHECK(false) << "invalid axis " << axis;
    }
  }

  float len2() const { return x * x + y * y + z * z; }

  float len() const { return sqrt(len2()); }

  void inormalize() { (*this) /= len(); }

  Vec3 normalize() const {
    Vec3 res = *this;
    res.inormalize();
    return res;
  }

  void imod(const Vec3& other) {
    if (other.x != 0) {
      x = fmod(x, other.x);
      if (x < 0) x += other.x;
    }
    if (other.y != 0) {
      y = fmod(y, other.y);
      if (y < 0) y += other.y;
    }
    if (other.z != 0) {
      z = fmod(z, other.z);
      if (z < 0) z += other.z;
    }
  }

  Vec3 mod(const Vec3& other) const {
    Vec3 res = *this;
    res.imod(other);
    return res;
  }

  float dot(const Vec3& other) const {
    return x * other.x + y * other.y + z * other.z;
  }

  void ireflect(const Vec3& other) { return *this -= (other * 2 * dot(other)); }

  Vec3 reflect(const Vec3& other) const {
    return *this - other * 2 * dot(other);
  }

  Vec3 cross(const Vec3& other) const {
    return Vec3(y * other.z - z * other.y, z * other.x - x * other.z,
                x * other.y - y * other.x);
  }

  Vec3 randomOrthonormalVec() const {
    Vec3 res = *this;
    if (x == 0) {
      res.x += 1;
    } else {
      res.y += 1;
    }
    res -= (*this) * this->dot(res);
    return res.normalize();
  }

  // Random unit vector.
  static Vec3 random() {
    Vec3 res = random_cube();
    float len2 = res.len2();
    while (len2 > 1) {
      res = random_cube();
      len2 = res.len2();
    }
    return res / sqrt(len2);
  }

  // Random vec in the unit sphere.
  static Vec3 random_sphere() {
    Vec3 res = random_cube();
    while (res.len2() > 1) {
      res = random_cube();
    }
    return res;
  }

  // Random vec in the unit cube.
  static Vec3 random_cube() {
    float x =
        -1 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (2)));
    float y =
        -1 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (2)));
    float z =
        -1 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (2)));
    return Vec3(x, y, z);
  }

  Vec3 clamp(float min, float max) const {
    return Vec3(std::clamp(x, min, max),
                std::clamp(y, min, max),
                std::clamp(z, min, max));
  }

  std::string str() const {
    std::stringstream res;
    res << "Vec3(" << x << ',' << y << ',' << z << ')';
    return res.str();
  }
};

typedef Vec3 vec3;

vec3 operator*(float alpha, const vec3& v) {
  return v * alpha;
}

vec3 abs(const vec3& v) { return vec3(abs(v.x), abs(v.y), abs(v.z)); }
float fract(float f) {
  float _;
  return modf(f, &_);
}
vec3 fract(const vec3& v) { return vec3(fract(v.x), fract(v.y), fract(v.z)); }
vec3 clamp(const vec3& v, float min, float max) { return v.clamp(min, max); }
vec3 mix(const vec3& a, const vec3& b, float f) {
  return a * f + b * (1 - f);
}

#endif
