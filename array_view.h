#ifndef ARRAY_VIEW_H
#define ARRAY_VIEW_H

#include <cstring>
#include <valarray>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

template <class value_type>
class ArrayView {
public:
    using Self = ArrayView<value_type>;
    using iterator = value_type *;
    using const_iterator = value_type const *;
    using reference = value_type &;
    using const_reference = value_type const &;

    // Ctors.
    constexpr ArrayView() {}

    constexpr ArrayView(value_type *raw_ptr, size_t size, size_t stride = 1) : data_(raw_ptr), size_(size), stride_(stride) {}

    template <typename container_type>
    constexpr ArrayView(container_type &container) noexcept
        : data_{&(*std::begin(container))}, size_{std::size(container)} {}

    // Iterators.
    iterator begin() {
        return data_;
    }
    iterator end() {
        return data_ + size_;
    }
    const_iterator begin() const {
        return data_;
    }
    const_iterator end() const {
        return data_ + size_;
    }
    const_iterator cbegin() const {
        return data_;
    }
    const_iterator cend() const {
        return data_ + size_;
    }

    constexpr std::size_t size() const noexcept { return size_; }
    constexpr std::size_t length() const noexcept { return size_; }

    // Indexed Access.
    reference operator[](std::size_t i) {
        return data_[index(i)];
    };
    const_reference operator[](std::size_t i) const {
        return data_[index(i)];
    };
    reference at(std::size_t i) {
        size_t idx = index(i);
        if (idx >= size_) {
            std::cerr << "ERROR: ArrayView: Invalid access to index " << i << " with size "
                      << size_ << std::endl;
        }
        return data_[idx];
    };
    const_reference at(std::size_t i) const {
        size_t idx = index(i);
        if (idx >= size_) {
            std::cerr << "ERROR: ArrayView: Invalid access to index " << i << " with size "
                      << size_ << std::endl;
        }
        return data_[idx];
    };

    // Slicing.
    Self slice(const std::slice &slice) const {
        return Self(data_ + slice.start(), slice.size(), stride_ * slice.stride());
    }
    Self operator[](const std::slice& slice) const {
        return this->slice(slice);
    };

    // Assignment.
    template <typename container_type>
    void operator=(const container_type& c) {
        if (c.size() != size()) {
            std::cerr << "ERROR: ArrayView: Invalid size for assignment " << c.size() << " vs "
                      << size() << std::endl;
        }
        for (size_t i = 0; i < size_; ++i) {
            (*this)[i] = c[i];
        }
    }

    template <typename container_type>
    container_type copy() {
        container_type res(size_);
        for (size_t i = 0; i < size_; ++i) {
            res[i] = (*this)[i];
        }
        return res;
    }

    // Debugging.
    std::string str(int cell_width = 0) const {
        std::stringstream res;
        res << "ArrayView(";
        for (size_t i = 0; i < size_; ++i) {
            res << std::setw(cell_width) << (*this)[i];
            if (i < size_ - 1) {
                res << ", ";
            }
        }
        res << ')';
        return res.str();
    }

private:
    constexpr size_t index(size_t i) const {
        return i * stride_;
    }

    value_type *data_ = 0;
    size_t size_ = 0;
    size_t stride_ = 1;
};

#endif
