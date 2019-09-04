#include <iostream>
#include <iomanip>
#include <vector>

#include "../array_view.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE( "ArrayViews are working", "[ArrayView]" ) {
    std::vector<int> vec;
    for (int i = 0; i < 10; ++i) {
        vec.push_back(i);
    }

    ArrayView<int> view(vec);
    REQUIRE(view.str() == "ArrayView(0, 1, 2, 3, 4, 5, 6, 7, 8, 9)");

    ArrayView<int> view2 = view[std::slice(0, 5, 2)];
    REQUIRE(view2.str() == "ArrayView(0, 2, 4, 6, 8)");

    ArrayView<int> view3 = view[std::slice(0, 3, 3)];
    REQUIRE(view3.str() == "ArrayView(0, 3, 6)");

    std::vector<int> copy = view3.copy<std::vector<int>>();
    std::cout << "view[slice(0, 3, 3)].copy(): " << std::endl;
    REQUIRE(copy == std::vector<int>{0, 3, 6});

    view3 = std::vector<int>{77, 88, 99};
    REQUIRE(view.str() == "ArrayView(77, 1, 2, 88, 4, 5, 99, 7, 8, 9)");
}
