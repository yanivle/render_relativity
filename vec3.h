#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <string>
#include <sstream>

struct Vec3 {
  float x, y, z;

  Vec3(): x(0), y(0), z(0) {}
  Vec3(float x, float y, float z): x(x), y(y), z(z) {}

  void operator+= (const Vec3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
  }

  void operator-= (const Vec3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
  }

  void operator*= (float alpha) {
    x *= alpha;
    y *= alpha;
    z *= alpha;
  }

  void operator/= (float alpha) {
    x /= alpha;
    y /= alpha;
    z /= alpha;
  }

  Vec3 operator+ (const Vec3& other) const {
    Vec3 res = *this;
    res += other;
    return res;
  }

  Vec3 operator- (const Vec3& other) const {
    Vec3 res = *this;
    res -= other;
    return res;
  }

  Vec3 operator- () const {
    return Vec3() - *this;
  }

  Vec3 operator* (float alpha) const {
    Vec3 res = *this;
    res *= alpha;
    return res;
  }

  Vec3 operator/ (float alpha) const {
    Vec3 res = *this;
    res /= alpha;
    return res;
  }

  float len2() const {
    return x*x + y*y + z*z;
  }

  float len() const {
    return sqrt(len2());
  }

  void inormalize() {
    (*this) /= len();
  }

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

  void ireflect(const Vec3& other) {
    return *this -= (other * 2 * dot(other));
  }

  Vec3 reflect(const Vec3& other) const {
    return *this - other * 2 * dot(other);
  }

  Vec3 cross(const Vec3& other) const {
    return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
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
    float x = -1 + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(2)));
    float y = -1 + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(2)));
    float z = -1 + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(2)));
    return Vec3(x, y, z);
  }

  std::string str() const {
    std::stringstream res;
    res << "Vec3(" << x << ',' << y << ',' << z << ')';
    return res.str();
  }
};

typedef Vec3 vec3;

#endif
