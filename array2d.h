#ifndef ARRAY2D_H
#define ARRAY2D_H

#include <cstring>
#include <valarray>
#include <iostream>

#include "array_view.h"

template <class value_type>
class Array2D {
public:
  // Ctors.
  Array2D(size_t width, size_t height) :
    width_(width), height_(height), size_(width * height) {
    data_ = new value_type[size_];
    std::memset(data_, 0, size_ * sizeof(value_type));
  }

  template <class other_value_type>
  Array2D(const Array2D<other_value_type>& other) :
    width_(other.width()), height_(other.height()), size_(other.size()) {
    data_ = new value_type[size_];
    for (int i = 0; i < size_; ++i) {
      data_[i] = other.data_[i];
    }
  }

  ~Array2D() {
    delete data_;
  }

  // Indexed access.
  value_type& operator()(int x, int y) {
    return data_[index(x, y)];
  }

  const value_type& operator()(int x, int y) const {
    return data_[index(x, y)];
  }

  value_type& operator()(int i) {
    return data_[i];
  }

  const value_type& operator()(int i) const {
    return data_[i];
  }

  value_type& at(int x, int y) {
    if (!is_safe(x, y)) {
      std::cerr << "ERROR: Array2D: unsafe access - x: " << x << " y: " << y << std::endl;
    }
    return data_[index(x, y)];
  }

  const value_type& at(int x, int y) const {
    if (!is_safe(x, y)) {
      std::cerr << "ERROR: Array2D: unsafe access - x: " << x << " y: " << y << std::endl;
    }
    return data_[index(x, y)];
  }

  value_type& at(int i) {
    if (!is_safe(i)) {
      std::cerr << "ERROR: Array2D: unsafe access - i: " << i << std::endl;
    }
    return data_[i];
  }

  const value_type& at(int i) const {
    if (!is_safe(i)) {
      std::cerr << "ERROR: Array2D: unsafe access - i: " << i << std::endl;
    }
    return data_[i];
  }

  // Row/Column access.
  ArrayView<value_type> row(int i) {
    return ArrayView<value_type>(data_ + index(0, i), width_, 1);
  }

  ArrayView<value_type> column(int i) {
    return ArrayView<value_type>(data_ + index(i, 0), height_, width_);
  }

  ArrayView<value_type> flatten() {
    return ArrayView<value_type>(data_, size_, 1);
  }

  unsigned int width() const { return width_; }
  unsigned int height() const { return height_; }
  unsigned int size() const { return size_; }

  // Debugging.
  void print(int cell_width=4) const {
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            std::cout << std::fixed << std::setprecision(2) << std::setw(cell_width) << (*this)(x, y) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
  }

private:
  inline int index(int x, int y) const {
    return (y * width_) + x;
  }

  bool is_safe(int x, int y) const {
    return x >= 0 && x < width_ && y >= 0 && y < height_;
  }

  bool is_safe(int i) const {
    return i >= 0 && i < size_;
  }

  size_t width_ = 0;
  size_t height_ = 0;
  size_t size_ = 0;
  value_type *data_ = 0;
};

#endif
