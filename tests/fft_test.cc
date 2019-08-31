#include <iostream>
#include <iomanip>
#include <vector>

#include "../array2d.h"
#include "../fft.h"

int main(void) {
    const int width = 4;
    const int height = 4;
    Array2D<std::complex<float>> arr(width, height);
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            arr(x, y) = x * y + x;
        }
    }

    std::cout << "Original: " << std::endl;
    arr.print(20);

    fft::fft2d(arr);

    std::cout << "After fft2d: " << std::endl;
    arr.print(20);

    fft::ifft2d(arr);

    std::cout << "After ifft2d: " << std::endl;
    arr.print(20);
}
