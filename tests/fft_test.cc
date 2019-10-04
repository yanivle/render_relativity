#include <iostream>
#include <iomanip>
#include <vector>

#include "../array2d.h"
#include "../fft.h"
#include "../image.h"
#include "../filters.h"

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
            } else if (arr(i).imag() != Approx(baseline_(i).imag()).margin(0.01)) {
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

class ImageApprox : public Catch::MatcherBase<Image> {
    Image baseline_;
public:
    ImageApprox(const Image& baseline) : baseline_(baseline) {}

    bool match(Image const& image) const override {
        if (image.width() != baseline_.width() || image.height() != baseline_.height()) {
            std::cerr << "Dimensions are different." << std::endl;
            return false;
        }
        for (int i = 0; i < image.size(); ++i) {
            bool approx_equal = true;
            if (image(i).r != Approx(baseline_(i).r).margin(0.01)) {
                approx_equal = false;
            } else if (image(i).g != Approx(baseline_(i).g).margin(0.01)) {
                approx_equal = false;
            } else if (image(i).b != Approx(baseline_(i).b).margin(0.01)) {
                approx_equal = false;
            }
            if (!approx_equal) {
                std::cerr << "Diff at position " << i << " " << image(i) << " vs "
                          << baseline_(i) << std::endl;
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

inline ImageApprox IsApproximatelyEqualTo(const Image& baseline) {
    return ImageApprox(baseline);
}

TEST_CASE("FFT for ints works", "[FFT]") {
    const int width = 4;
    const int height = 4;
    Array2D<Complex> arr(width, height);
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            arr(x, y) = x * y + x;
        }
    }
    Array2D<Complex> original = arr;
    Array2D<Complex> golden_fft_result{
        {Complex(60, 0), Complex(-20, 20), Complex(-20, 0), Complex(-20, -20)},
        {Complex(-12, 12), Complex(-0.00000047683715820312, -8), Complex(4, -4), Complex(8, 0)},
        {Complex(-12, 0), Complex(4, -4), Complex(4, 0), Complex(4, 4)},
        {Complex(-12, -12), Complex(8, -0.00000047683715820312), Complex(4, 4), Complex(0.00000071525573730469, 8)}
    };

    SECTION("Single threaded") {
        fft::fft2d(arr);
        CHECK_THAT(arr, IsApproximatelyEqualTo(golden_fft_result));
        fft::ifft2d(arr);
        CHECK_THAT(arr, IsApproximatelyEqualTo(original));
    }

    SECTION("Multi threaded") {
        fft::fft2d_mt(arr);
        CHECK_THAT(arr, IsApproximatelyEqualTo(golden_fft_result));
        fft::ifft2d_mt(arr);
        CHECK_THAT(arr, IsApproximatelyEqualTo(original));
    }
}

TEST_CASE("FFT 2D Works", "[FFT]") {
  INFO("Creating image...");
  Image image(1024, 1024);
  std::vector<int> freqs{2, 5, 17, 50};
  for (int y = 0; y < 1024; ++y) {
    for (int x = 0; x < 1024; ++x) {
      float v = 0;
      for (const int& freq : freqs) {
        v += sin(x * M_PI * 2 * freq / 1024) + cos(y * M_PI * 2 * freq / 1024);
      }
      v *= 100;
      image(x, y) = Color(v, 0, 0);
    }
  }
  INFO("Saving image...");
  image.save("/tmp/fft_test0.ppm");

  INFO("Extracting channel...");
  Array2D<fft::Complex> r = image.getChannel(Image::RED);
  INFO("FFT2D...");
  fft::fft2d(r);

  INFO("Tansforming and setting channels...");
  Image image2(1024, 1024);
  Array2D<float> rr = r.transform<float>([](fft::Complex c) {return c.real();});
  image2.setChannel(Image::RED, rr);
  Array2D<float> gg = r.transform<float>([](fft::Complex c) {return c.imag();});
  image2.setChannel(Image::GREEN, gg);
  INFO("Saving image...");
  image2.save("/tmp/fft_test1.ppm");

  INFO("IFFT2D...");
  fft::ifft2d(r);

  Image image3(1024, 1024);
  INFO("Tansforming and setting channel...");
  rr = r.transform<float>([](fft::Complex c) {return c.real();});
  image3.setChannel(Image::RED, rr);
  INFO("Saving image...");
  image3.save("/tmp/fft_test2.ppm");

  CHECK_THAT(image3, IsApproximatelyEqualTo(image));
}

TEST_CASE("Bloom convolution works", "[FFT]") {
  Image image(1024, 1024);
  std::vector<int> freqs{2, 5, 17, 50};
  for (int y = 0; y < 1024; ++y) {
    for (int x = 0; x < 1024; ++x) {
      float v = 0;
      for (const int& freq : freqs) {
        v += sin(x * M_PI * 2 * freq / 1024) + cos(y * M_PI * 2 * freq / 1024);
      }
      v *= (1000 / 255);
      if (v < 0) v = 0;
      image(x, y) = Color(v, 0, 0);
    }
  }
  INFO("Saving image...");
  image.save("/tmp/convolution_test0.ppm");

  INFO("Convolving...");
  filters::Convolve(image, filters::Bloom(1024, 1024));

  INFO("Saving image...");
  image.save("/tmp/convolution_test1.ppm");

  Image golden(1024, 1024);
  golden.deserialize("tests/golden_bloom.img");
  CHECK_THAT(image, IsApproximatelyEqualTo(golden));
}

TEST_CASE("Bloom convolution single point works", "[FFT]") {
  Image image(1024, 1024);
  image(300, 300) = Color(10000, 0, 0);
  INFO("Saving image...");
  image.save("/tmp/convolution_single_point_test0.ppm");

  INFO("Convolving...");
  filters::Convolve(image, filters::Bloom(1024, 1024));

  INFO("Saving image...");
  image.save("/tmp/convolution_single_point_test1.ppm");

  Image golden(1024, 1024);
  golden.deserialize("tests/golden_bloom_single_point.img");
  CHECK_THAT(image, IsApproximatelyEqualTo(golden));
}
