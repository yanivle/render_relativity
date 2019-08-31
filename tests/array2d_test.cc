#include <iostream>
#include <iomanip>

#include "../array2d.h"

int main(void) {
    Array2D<int> arr(10, 10);
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            arr(x, y) = x * y + x;
        }
    }

    arr.print();

    const auto c = arr.column(2);
    std::cout << "Col(2): " << c.str() << std::endl;

    const auto r = arr.row(2);
    std::cout << "Row(2): " << r.str() << std::endl;

    const auto c2 = c[std::slice(0, 5, 2)];
    std::cout << "Col(2)[slice(0, 5, 2)]: " << c2.str() << std::endl;

    const auto r2 = r[std::slice(1, 5, 2)];
    std::cout << "Row(2)[slice(1, 5, 2)]: " << r2.str() << std::endl;

    const auto c3 = c2[std::slice(0, 3, 2)];
    std::cout << "Col(2)[slice(0, 5, 2)][slice(0, 3, 2)]: " << c3.str() << std::endl;
}
