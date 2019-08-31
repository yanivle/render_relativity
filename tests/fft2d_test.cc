#include <iostream>
#include <iomanip>
#include <vector>

#include "../array2d.h"
#include "../image.h"
#include "../fft.h"

int main(void) {
  std::cout << "Creating image..." << std::endl;
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
  std::cout << "Saving image..." << std::endl;
  image.save("/tmp/fft_test0.ppm");

  std::cout << "Extracting channel..." << std::endl;
  Array2D<fft::Complex> r = image.getChannel(Image::RED);
  std::cout << "FFT2D..." << std::endl;
  fft::fft2d(r);

  std::cout << "Tansforming and setting channels..." << std::endl;
  Image image2(1024, 1024);
  Array2D<float> rr = r.transform<float>([](fft::Complex c) {return c.real();});
  image2.setChannel(Image::RED, rr);
  Array2D<float> gg = r.transform<float>([](fft::Complex c) {return c.imag();});
  image2.setChannel(Image::GREEN, gg);
  std::cout << "Saving image..." << std::endl;
  image2.save("/tmp/fft_test1.ppm");

  std::cout << "IFFT2D..." << std::endl;
  fft::ifft2d(r);

  Image image3(1024, 1024);
  std::cout << "Tansforming and setting channel..." << std::endl;
  rr = r.transform<float>([](fft::Complex c) {return c.real();});
  image3.setChannel(Image::RED, rr);
  std::cout << "Saving image..." << std::endl;
  image3.save("/tmp/fft_test2.ppm");

  return 0;
}
