#include <iostream>
#include <iomanip>

#include "../array2d.h"
#include "../fft.h"

typedef Array2D<fft::Complex> ComplexArray2D;

#include "catch.hpp"

TEST_CASE("Array2D Scalar Operations", "[Array2D]") {
    ComplexArray2D arr(10, 10);
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            arr(x, y) = x * y + x;
        }
    }
    CHECK(arr.sum().real() == Approx(2475));
    CHECK(arr.sum().imag() == Approx(0));

    arr.inormalize();
    CHECK(arr.sum().real() == Approx(1));
    CHECK(arr.sum().imag() == Approx(0));

    ComplexArray2D top_left(10, 10);
    top_left(0, 0) = 1;
    arr.iaverage(top_left);

    CHECK(arr(0, 0).real() == Approx(0.5));
    CHECK(arr(0, 0).imag() == Approx(0));

    CHECK(arr(9, 9).real() == Approx(0.0181818176));
    CHECK(arr(9, 9).imag() == Approx(0));

    CHECK(arr.sum().real() == Approx(1));
    CHECK(arr.sum().imag() == Approx(0));
}
