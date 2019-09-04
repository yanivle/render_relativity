#include <iostream>
#include <iomanip>

#include "../array2d.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE( "Array2D Works", "[Array2D]" ) {
    Array2D<int> arr(10, 10);
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            arr(x, y) = x * y + x;
        }
    }

    const auto c = arr.column(2);
    REQUIRE(c.str() == "ArrayView(2, 4, 6, 8, 10, 12, 14, 16, 18, 20)");

    const auto r = arr.row(2);
    REQUIRE(r.str() == "ArrayView(0, 3, 6, 9, 12, 15, 18, 21, 24, 27)");

    const auto c2 = c[std::slice(0, 5, 2)];
    REQUIRE(c2.str() == "ArrayView(2, 6, 10, 14, 18)");

    const auto r2 = r[std::slice(1, 5, 2)];
    REQUIRE(r2.str() == "ArrayView(3, 9, 15, 21, 27)");

    const auto c3 = c2[std::slice(0, 3, 2)];
    REQUIRE(c3.str() == "ArrayView(2, 10, 18)");
}
