#include <iostream>
#include <iomanip>
#include <vector>

#include "../array2d.h"
#include "../fft.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

typedef std::complex<float> Complex;

class Array2DApprox : public Catch::MatcherBase<Array2D<Complex>> {
    Array2D<Complex> baseline_;
public:
    Array2DApprox(const Array2D<Complex>& baseline) : baseline_(baseline) {}

    bool match(Array2D<Complex> const& arr) const override {
        if (arr.width() != baseline_.width() || arr.height() != baseline_.height()) {
            return false;
        }
        for (int i = 0; i < arr.size(); ++i) {
            if (arr(i).real() != Approx(baseline_(i).real()).margin(0.01)) {
                return false;
            }
            if (arr(i).imag() != Approx(baseline_(i).imag()).margin(0.01)) {
                return false;
            }
        }
        return true;
    }

    virtual std::string describe() const override {
        std::ostringstream ss;
        ss << "is approximately equal to " << baseline_.str();
        return ss.str();
    }
};

inline Array2DApprox IsApproximatelyEqualTo(const Array2D<Complex>& baseline) {
    return Array2DApprox(baseline);
}

TEST_CASE( "FFT for ints works", "[FFT]" ) {
    const int width = 4;
    const int height = 4;
    Array2D<Complex> arr(width, height);
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            arr(x, y) = x * y + x;
        }
    }
    Array2D<Complex> original = arr;

    fft::fft2d(arr);
    Array2D<Complex> golden_fft_result{
        {Complex(60, 0), Complex(-20, 20), Complex(-20, 0), Complex(-20, -20)},
        {Complex(-12, 12), Complex(-0.00000047683715820312, -8), Complex(4, -4), Complex(8, 0)},
        {Complex(-12, 0), Complex(4, -4), Complex(4, 0), Complex(4, 4)},
        {Complex(-12, -12), Complex(8, -0.00000047683715820312), Complex(4, 4), Complex(0.00000071525573730469, 8)}
    };

    CHECK_THAT(arr, IsApproximatelyEqualTo(golden_fft_result));

    fft::ifft2d(arr);

    CHECK_THAT(arr, IsApproximatelyEqualTo(original));
}
