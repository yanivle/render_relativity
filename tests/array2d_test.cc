#include <iostream>
#include <iomanip>

#include "../array2d.h"

#include "catch.hpp"

TEST_CASE("Array2D Works", "[Array2D]") {
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

TEST_CASE("Serialize/deserialize work", "[Array2D]") {
    Array2D<int> arr(10, 10);
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            arr(x, y) = x * y + x;
        }
    }

    arr.serialize("/tmp/delme");
    Array2D<int> loaded(10, 10);
    loaded.deserialize("/tmp/delme");

    CHECK(arr == loaded);
}

TEST_CASE("Resize", "[Array2D]") {
    Array2D<int> arr({{1, 2, 3},
                      {4, 5, 6},
                      {7, 8, 9}});

    Array2D smaller({{1, 2},
                     {4, 5}});

    Array2D<int> larger({{1, 2, 3, 0},
                         {4, 5, 6, 0},
                         {7, 8, 9, 0},
                         {0, 0, 0, 0}});

    CHECK(arr.resize(2, 2) == smaller);
    CHECK(arr.resize(4, 4) == larger);
}
