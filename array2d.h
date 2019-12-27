#ifndef ARRAY2D_H
#define ARRAY2D_H

#include <cstring>
#include <functional>
#include <iostream>
#include <valarray>

#include "array_view.h"

template <class value_type>
class Array2D {
 public:
  template <class>
  friend class Array2D;

  // Ctors.
  Array2D(size_t width, size_t height)
      : width_(width), height_(height), size_(width * height) {
    data_ = new value_type[size_]();
  }

  template <class other_value_type>
  Array2D(const Array2D<other_value_type>& other)
      : width_(other.width()), height_(other.height()), size_(other.size()) {
    data_ = new value_type[size_];
    for (int i = 0; i < size_; ++i) {
      data_[i] = other.data_[i];
    }
  }

  template <class other_value_type>
  Array2D<other_value_type> transform(
      const std::function<other_value_type(value_type)>& f) {
    Array2D<other_value_type> res(width(), height());
    for (int i = 0; i < size_; ++i) {
      res.data_[i] = f(data_[i]);
    }
    return res;
  }

  Array2D(const std::initializer_list<std::initializer_list<value_type>>& lst) {
    height_ = lst.size();
    width_ = lst.begin()->size();
    size_ = width_ * height_;
    data_ = new value_type[size_];
    int i = 0;
    for (const auto& l : lst) {
      CHECK(l.size() == width_)
          << "Initializing Array2D from inconsistent initializer_list.";
      for (const auto& v : l) {
        data_[i] = v;
        i++;
      }
    }
  }

  Array2D(const Array2D<value_type>& other)
      : width_(other.width()), height_(other.height()), size_(other.size()) {
    data_ = new value_type[size_];
    for (int i = 0; i < size_; ++i) {
      data_[i] = other.data_[i];
    }
  }

  Array2D(Array2D<value_type>&& other) { *this = std::move(other); }

  Array2D& operator=(Array2D&& other) {
    if (this != &other) {
      std::swap(width_, other.width_);
      std::swap(height_, other.height_);
      std::swap(size_, other.size_);
      std::swap(data_, other.data_);
    }
    return *this;
  }

  ~Array2D() { delete[] data_; }

  // Comparison.
  bool operator==(const Array2D& other) const {
    if (width_ != other.width_ || height_ != other.height_) {
      return false;
    }
    for (int i = 0; i < size_; ++i) {
      if (data_[i] != other.data_[i]) {
        return false;
      }
    }
    return true;
  }

  // Indexed access.
  value_type& operator()(int x, int y) { return data_[index(x, y)]; }

  const value_type& operator()(int x, int y) const {
    return data_[index(x, y)];
  }

  value_type& operator()(int i) { return data_[i]; }

  const value_type& operator()(int i) const { return data_[i]; }

  value_type& at(int x, int y) {
    CHECK(is_safe(x, y)) << "unsafe access - x: " << x << " y: " << y;
    return data_[index(x, y)];
  }

  const value_type& at(int x, int y) const {
    CHECK(is_safe(x, y)) << "unsafe access - x: " << x << " y: " << y;
    return data_[index(x, y)];
  }

  value_type& at(int i) {
    CHECK(is_safe(i)) << "unsafe access - i: " << i;
    return data_[i];
  }

  const value_type& at(int i) const {
    CHECK(is_safe(i)) << "unsafe access - i: " << i;
    return data_[i];
  }

  // Row/Column access.
  ArrayView<value_type> row(int i) const {
    return ArrayView<value_type>(data_ + index(0, i), width_, 1);
  }

  ArrayView<value_type> column(int i) const {
    return ArrayView<value_type>(data_ + index(i, 0), height_, width_);
  }

  ArrayView<value_type> flatten() const {
    return ArrayView<value_type>(data_, size_, 1);
  }

  size_t width() const { return width_; }
  size_t height() const { return height_; }
  size_t size() const { return size_; }

  Array2D<value_type> rotate(int x_rotation, int y_rotation) {
    Array2D<value_type> res(width(), height());
    for (int y = 0; y < height(); ++y) {
      for (int x = 0; x < width(); ++x) {
        res(x, y) =
            (*this)((x + x_rotation) % width(), (y + y_rotation) % height());
      }
    }
    return res;
  }

  // Resizing.
  Array2D<value_type> resize(size_t width, size_t height) const {
    Array2D<value_type> res(width, height);
    size_t w = std::min(width_, width);
    size_t h = std::min(height_, height);
    for (int y = 0; y < h; ++y) {
      for (int x = 0; x < w; ++x) {
        res(x, y) = (*this)(x, y);
      }
    }
    return res;
  }

  // Debugging.
  void print(int cell_width = 4, int precision = 2) const {
    for (int y = 0; y < height_; ++y) {
      for (int x = 0; x < width_; ++x) {
        std::cout << std::fixed << std::setprecision(precision)
                  << std::setw(cell_width) << (*this)(x, y) << " ";
      }
      std::cout << std::endl;
    }
  }

  std::string str(const int max_dim = 3) const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Array2D(" << std::endl;
    for (int y = 0; y < height(); ++y) {
      if (y >= max_dim) {
        ss << "...";
        break;
      }
      for (int x = 0; x < width(); ++x) {
        if (x >= max_dim) {
          ss << "...";
          break;
        }
        ss << (*this)(x, y);
        if (x < width() - 1) {
          ss << ", ";
        }
      }
      ss << std::endl;
    }
    ss << ")";
    return ss.str();
  }

  // Serialization.
  void serialize(const std::string& filename) const {
    std::ofstream file(filename, std::ofstream::binary);
    file.write((char*)&width_, sizeof(width_));
    file.write((char*)&height_, sizeof(height_));
    file.write((char*)&size_, sizeof(size_));
    file.write((char*)data_, sizeof(value_type) * size_);
    file.close();
  }

  void deserialize(const std::string& filename) const {
    std::ifstream file(filename, std::ifstream::binary);
    file.read((char*)&width_, sizeof(width_));
    file.read((char*)&height_, sizeof(height_));
    file.read((char*)&size_, sizeof(size_));
    file.read((char*)data_, sizeof(value_type) * size_);
    file.close();
  }

  // Scalar operations.
  void operator*=(value_type a) {
    for (int i = 0; i < size(); ++i) {
      data_[i] *= a;
    }
  }

  void operator*=(const Array2D<value_type>& other) {
    for (int i = 0; i < size(); ++i) {
      data_[i] *= other.data_[i];
    }
  }

  void operator/=(value_type a) {
    for (int i = 0; i < size(); ++i) {
      data_[i] /= a;
    }
  }

  value_type sum() const {
    value_type sum = 0;
    for (int i = 0; i < size(); ++i) {
      sum += data_[i];
    }
    return sum;
  }

  value_type min() const {
    const auto flat = flatten();
    return *std::min_element(flat.cbegin(), flat.cend());
  }

  value_type max() const {
    const auto flat = flatten();
    return *std::max_element(flat.cbegin(), flat.cend());
  }

  void operator+=(const Array2D<value_type>& other) {
    for (int i = 0; i < size(); ++i) {
      data_[i] += other.data_[i];
    }
  }

  void inormalize() { (*this) /= sum(); }

  void iaverage(const Array2D<value_type>& other) {
    (*this) += other;
    (*this) /= 2;
  }

 protected:
  inline int index(int x, int y) const { return (y * width_) + x; }

  bool is_safe(int x, int y) const {
    return x >= 0 && x < width_ && y >= 0 && y < height_;
  }

  bool is_safe(int i) const { return i >= 0 && i < size_; }

  size_t width_ = 0;
  size_t height_ = 0;
  size_t size_ = 0;
  value_type* data_ = 0;
};

template <class value_type>
std::ostream& operator<<(std::ostream& os, Array2D<value_type> const& arr) {
  return os << arr.str();
}

#endif
