#ifndef MATRIX_H
#define MATRIX_H

template <class T>
class Matrix {
public:
  Matrix(int width, int height) :
  width_(width), height_(height), size_(width * height) {
    data_ = new T[size_];
    memset(data_, 0, size_);
  }

  ~Matrix() {
    delete data_;
  }

  T& operator()(int x, int y) {
    if (!is_safe(x, y)) {
      std::cerr << "ERROR: unsafe access - x: " << x << " y: " << y << std::endl;
    }
    return data_[y * width_ + x];
  }

  T& operator()(int i) {
    return data_[i];
  }

  unsigned int size() {
    return size_;
  }

  bool is_safe(int x, int y) {
    return x >= 0 && x < width_ && y >= 0 && y < height_;
  }

  void safe_set(int x, int y, const T& val) {
    if (is_safe(x, y)) (*this)(x, y) = val;
  }

  void scale_x(int factor, Matrix<T>* res) {
    if (width_ * factor != res->width_) {
      std::cerr << "ERROR: Trying to x-scale matrix of width " << width_
                << " with a factor of " << factor << " into a matrix of width "
                << res->width_ << std::endl;
      return;
    }
    for (int x = 0; x < width_; ++x) {
      for (int y = 0; y < height_; ++y) {
        const T& val = (*this)(x, y);
        for (int i = 0; i < factor; ++i) {
          (*res)(x * factor + i, y) = val;
        }
      }
    }
  }

  void flip_x(Matrix<T>* res) {
    if (!is_compatible(*res)) {
      std::cerr << "ERROR: Trying to x-flip incompatible matrices: ("
                << width_ << ", " << height_ << ") (" << res->width_ << ", "
                << res->height_ << std::endl;
      return;
    }

    for (int y = 0; y < height_; ++y) {
      for (int x = 0; x < width_; ++x) {
        (*res)(width_ - 1 - x, y) = (*this)(x, y);
      }
    }
  }

  template<class S>
  bool is_compatible(const Matrix<S>& other) {
    return width_ == other.width_ && height_ == other.height_;
  }

// protected:
  unsigned int width_;
  unsigned int height_;
  unsigned int size_;
  T *data_;
};

#endif
