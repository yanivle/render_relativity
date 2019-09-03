#ifndef FFT_H
#define FFT_H

#include <complex>
#include <valarray>
#include <thread>

namespace fft {

typedef std::complex<float> Complex;
typedef std::valarray<Complex> ComplexArray;

inline void fft(ComplexArray& x) {
    const size_t N = x.size();
    if (N <= 1) return;

    // divide
    ComplexArray even = x[std::slice(0, N/2, 2)];
    ComplexArray  odd = x[std::slice(1, N/2, 2)];

    // conquer
    fft(even);
    fft(odd);

    // combine
    for (size_t k = 0; k < N/2; ++k)
    {
        Complex t = std::polar<float>(1.0, -2 * M_PI * k / N) * odd[k];
        x[k    ] = even[k] + t;
        x[k+N/2] = even[k] - t;
    }
}

inline void ifft(ComplexArray& x) {
    x = x.apply(std::conj);
    fft( x );
    x = x.apply(std::conj);
    x /= x.size();
}

namespace fft_mt {

template<bool horizontal, bool forward>
void fft2d_thread(Array2D<Complex>* arr, int block_min, int block_max) {
    // std::cout << __FUNCTION__ << " min " << block_min << " max " << block_max << std::endl;
    for (int i = block_min; i < block_max; ++i) {
        ComplexArray slice;
        if constexpr (horizontal) {
            slice = arr->row(i).template copy<ComplexArray>();
        } else {
            slice = arr->column(i).template copy<ComplexArray>();
        }
        if constexpr (forward) {
            fft(slice);
        } else {
            ifft(slice);
        }
        if constexpr (horizontal) {
            arr->row(i) = slice;
        } else {
            arr->column(i) = slice;
        }
    }
}

}

void fft2d_mt(Array2D<Complex>& arr) {
    const int num_threads = std::thread::hardware_concurrency();
    int row_block_size = arr.height() / num_threads;
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
      int block_min = i * row_block_size;
      int block_max = block_min + row_block_size;
      if (i == num_threads - 1) {
          block_max = arr.height();
      }
      threads.push_back(std::thread(fft_mt::fft2d_thread<true, true>, &arr, block_min, block_max));
    }
    for (std::thread& thread : threads) {
      thread.join();
    }

    threads.clear();
    int col_block_size = arr.width() / num_threads;
    for (int i = 0; i < num_threads; ++i) {
      int block_min = i * col_block_size;
      int block_max = block_min + col_block_size;
      if (i == num_threads - 1) {
          block_max = arr.width();
      }
      threads.push_back(std::thread(fft_mt::fft2d_thread<false, true>, &arr, block_min, block_max));
    }
    for (std::thread& thread : threads) {
      thread.join();
    }
}

void ifft2d_mt(Array2D<Complex>& arr) {
    const int num_threads = std::thread::hardware_concurrency();
    int row_block_size = arr.height() / num_threads;
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
      int block_min = i * row_block_size;
      int block_max = block_min + row_block_size;
      if (i == num_threads - 1) {
          block_max = arr.height();
      }
      threads.push_back(std::thread(fft_mt::fft2d_thread<true, false>, &arr, block_min, block_max));
    }
    for (std::thread& thread : threads) {
      thread.join();
    }

    threads.clear();
    int col_block_size = arr.width() / num_threads;
    for (int i = 0; i < num_threads; ++i) {
      int block_min = i * col_block_size;
      int block_max = block_min + col_block_size;
      if (i == num_threads - 1) {
          block_max = arr.width();
      }
      threads.push_back(std::thread(fft_mt::fft2d_thread<false, false>, &arr, block_min, block_max));
    }
    for (std::thread& thread : threads) {
      thread.join();
    }
}

void fft2d(Array2D<Complex>& arr) {
    for (int y = 0; y < arr.height(); ++y) {
        ComplexArray row = arr.row(y).template copy<ComplexArray>();
        fft(row);
        arr.row(y) = row;
    }
    for (int x = 0; x < arr.width(); ++x) {
        ComplexArray col = arr.column(x).template copy<ComplexArray>();
        fft(col);
        arr.column(x) = col;
    }
}

void ifft2d(Array2D<Complex>& arr) {
    for (int x = 0; x < arr.width(); ++x) {
        ComplexArray col = arr.column(x).template copy<ComplexArray>();
        ifft(col);
        arr.column(x) = col;
    }
    for (int y = 0; y < arr.height(); ++y) {
        ComplexArray row = arr.row(y).template copy<ComplexArray>();
        ifft(row);
        arr.row(y) = row;
    }
}

}

#endif

