#include <iostream>
#include <iomanip>
#include <vector>

#include "../array2d.h"
#include "../fft.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE( "FFT for ints works", "[FFT]" ) {
    const int width = 4;
    const int height = 4;
    Array2D<std::complex<float>> arr(width, height);
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            arr(x, y) = x * y + x;
        }
    }
    Array2D<std::complex<float>> original = arr;

    fft::fft2d(arr);
    arr.print(20, 100);
    CHECK(arr(0, 0).real() == Approx(60));
    CHECK(arr(0, 0).imag() == Approx(0));
    CHECK(arr(0, 1).real() == Approx(-12));
    CHECK(arr(0, 1).imag() == Approx(12));
    CHECK(arr(0, 2).real() == Approx(-12));
    CHECK(arr(0, 2).imag() == Approx(0));
    CHECK(arr(0, 3).real() == Approx(-12));
    CHECK(arr(0, 3).imag() == Approx(-12));
    CHECK(arr(1, 0).real() == Approx(-20));
    CHECK(arr(1, 0).imag() == Approx(20));
    CHECK(arr(1, 1).real() == Approx(-0.00000047683715820312));
    CHECK(arr(1, 1).imag() == Approx(-8));
    CHECK(arr(1, 2).real() == Approx(4));
    CHECK(arr(1, 2).imag() == Approx(-4));
    CHECK(arr(1, 3).real() == Approx(8));
    CHECK(arr(1, 3).imag() == Approx(0.00000000000000000000));
    CHECK(arr(2, 0).real() == Approx(-20));
    CHECK(arr(2, 0).imag() == Approx(0));
    CHECK(arr(2, 1).real() == Approx(4));
    CHECK(arr(2, 1).imag() == Approx(-4));
    CHECK(arr(2, 2).real() == Approx(4));
    CHECK(arr(2, 2).imag() == Approx(0));
    CHECK(arr(2, 3).real() == Approx(4));
    CHECK(arr(2, 3).imag() == Approx(4));
    CHECK(arr(3, 0).real() == Approx(-20));
    CHECK(arr(3, 0).imag() == Approx(-20));
    CHECK(arr(3, 1).real() == Approx(8));
    CHECK(arr(3, 1).imag() == Approx(0));
    CHECK(arr(3, 2).real() == Approx(4));
    CHECK(arr(3, 2).imag() == Approx(4));
    CHECK(arr(3, 3).real() == Approx(0.00000071525573730469));
    CHECK(arr(3, 3).imag() == Approx(8));

    fft::ifft2d(arr);
    CHECK(arr == original);
}
