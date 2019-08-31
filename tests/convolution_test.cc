#include <iostream>
#include <iomanip>
#include <vector>

#include "../array2d.h"
#include "../image.h"
#include "../filters.h"

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
      if (v < 0) v = 0;
      image(x, y) = Color(v, 0, 0);
    }
  }
  std::cout << "Saving image..." << std::endl;
  image.save("/tmp/convolution_test0.ppm");

  std::cout << "Convolving..." << std::endl;
  filters::Convolve(image, filters::Bloom(1024, 1024));

  std::cout << "Saving image..." << std::endl;
  image.save("/tmp/convolution_test1.ppm");

  return 0;
}
