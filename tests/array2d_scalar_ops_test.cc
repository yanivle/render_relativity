#include <iostream>
#include <iomanip>

#include "../array2d.h"
#include "../fft.h"

typedef Array2D<fft::Complex> ComplexArray2D;

int main(void) {
    ComplexArray2D arr(10, 10);
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            arr(x, y) = x * y + x;
        }
    }
    arr.print();
    std::cout << "Sum: " << arr.sum() << std::endl;

    arr.inormalize();
    std::cout << "Normalized: " << std::endl;
    arr.print();
    std::cout << "Sum: " << arr.sum() << std::endl;

    ComplexArray2D top_left(10, 10);
    top_left(0, 0) = 1;
    std::cout << "Top-left: " << std::endl;
    top_left.print();

    arr.iaverage(top_left);
    std::cout << "Averaged: " << std::endl;
    arr.print();
    std::cout << "Sum: " << arr.sum() << std::endl;
}
